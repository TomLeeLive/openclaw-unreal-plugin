// Copyright OpenClaw. All Rights Reserved.

#include "OpenClawConnectionManager.h"
#include "OpenClawTools.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Async/Async.h"
#include "Misc/App.h"

DEFINE_LOG_CATEGORY(LogOpenClaw);

FOpenClawConnectionManager* FOpenClawConnectionManager::Instance = nullptr;

FOpenClawConnectionManager::FOpenClawConnectionManager()
	: GatewayHost(TEXT("127.0.0.1"))
	, GatewayPort(18789)  // OpenClaw Gateway default port
	, bAutoConnect(true)
{
}

FOpenClawConnectionManager::~FOpenClawConnectionManager()
{
}

FOpenClawConnectionManager& FOpenClawConnectionManager::Get()
{
	if (!Instance)
	{
		Instance = new FOpenClawConnectionManager();
	}
	return *Instance;
}

void FOpenClawConnectionManager::Initialize()
{
	UE_LOG(LogOpenClaw, Log, TEXT("Initializing OpenClaw connection manager..."));
	
	LoadConfig();
	
	if (bAutoConnect)
	{
		Connect();
	}
}

void FOpenClawConnectionManager::Shutdown()
{
	UE_LOG(LogOpenClaw, Log, TEXT("Shutting down OpenClaw connection manager..."));
	
	Disconnect();
	
	if (Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}

void FOpenClawConnectionManager::LoadConfig()
{
	// Try to load from project config
	FString ConfigPath = FPaths::ProjectDir() / TEXT("openclaw.json");
	
	// Fall back to user home directory
	if (!FPaths::FileExists(ConfigPath))
	{
		ConfigPath = FPlatformMisc::GetEnvironmentVariable(TEXT("HOME")) / TEXT(".openclaw") / TEXT("unreal-plugin.json");
	}
	
	if (FPaths::FileExists(ConfigPath))
	{
		FString JsonString;
		if (FFileHelper::LoadFileToString(JsonString, *ConfigPath))
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
			
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				if (JsonObject->HasField(TEXT("host")))
				{
					GatewayHost = JsonObject->GetStringField(TEXT("host"));
				}
				if (JsonObject->HasField(TEXT("port")))
				{
					GatewayPort = JsonObject->GetIntegerField(TEXT("port"));
				}
				if (JsonObject->HasField(TEXT("secret")))
				{
					PluginSecret = JsonObject->GetStringField(TEXT("secret"));
				}
				if (JsonObject->HasField(TEXT("autoConnect")))
				{
					bAutoConnect = JsonObject->GetBoolField(TEXT("autoConnect"));
				}
				
				UE_LOG(LogOpenClaw, Log, TEXT("Loaded config from: %s"), *ConfigPath);
			}
		}
	}
	else
	{
		UE_LOG(LogOpenClaw, Log, TEXT("Using default config (Gateway: %s:%d)"), *GatewayHost, GatewayPort);
	}
}

void FOpenClawConnectionManager::Connect()
{
	if (State == EOpenClawConnectionState::Connected || State == EOpenClawConnectionState::Connecting)
	{
		return;
	}
	
	SetState(EOpenClawConnectionState::Connecting);
	
	UE_LOG(LogOpenClaw, Log, TEXT("Connecting to OpenClaw Gateway at %s:%d..."), *GatewayHost, GatewayPort);
	
	// Register with the Gateway
	SendRegister();
}

void FOpenClawConnectionManager::Disconnect()
{
	if (State == EOpenClawConnectionState::Disconnected)
	{
		return;
	}
	
	StopPolling();
	SessionId.Empty();
	SetState(EOpenClawConnectionState::Disconnected);
	
	UE_LOG(LogOpenClaw, Log, TEXT("Disconnected from OpenClaw Gateway"));
}

void FOpenClawConnectionManager::SendRegister()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildUrl(TEXT("/unreal/register")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	// Get project name from project settings
	FString ProjectName = FApp::GetProjectName();
	if (ProjectName.IsEmpty())
	{
		ProjectName = TEXT("UnrealProject");
	}
	
	// Get engine version
	FString EngineVersion = FString::Printf(TEXT("%d.%d"), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION);
	
	// Build registration body
	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("project"), ProjectName);
	Body->SetStringField(TEXT("version"), EngineVersion);
	Body->SetStringField(TEXT("platform"), TEXT("UnrealEditor"));
	Body->SetNumberField(TEXT("tools"), FOpenClawTools::GetToolCount());
	
	FString BodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);
	
	Request->SetContentAsString(BodyString);
	Request->OnProcessRequestComplete().BindRaw(this, &FOpenClawConnectionManager::HandleRegisterResponse);
	Request->ProcessRequest();
}

void FOpenClawConnectionManager::HandleRegisterResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (!bSuccess || !Response.IsValid())
	{
		UE_LOG(LogOpenClaw, Warning, TEXT("Failed to connect to OpenClaw Gateway"));
		SetState(EOpenClawConnectionState::Reconnecting);
		// Will retry on next tick cycle
		return;
	}
	
	int32 ResponseCode = Response->GetResponseCode();
	
	if (ResponseCode == 200)
	{
		// Parse response to get session ID
		FString ResponseBody = Response->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
		
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			if (JsonObject->HasField(TEXT("sessionId")))
			{
				SessionId = JsonObject->GetStringField(TEXT("sessionId"));
				UE_LOG(LogOpenClaw, Log, TEXT("Registered with session ID: %s"), *SessionId);
				
				SetState(EOpenClawConnectionState::Connected);
				StartPolling();
			}
		}
	}
	else
	{
		UE_LOG(LogOpenClaw, Warning, TEXT("Registration failed with code: %d"), ResponseCode);
		SetState(EOpenClawConnectionState::Error);
	}
}

void FOpenClawConnectionManager::StartPolling()
{
	bIsPolling = true;
	TimeSinceLastPoll = PollInterval; // Trigger immediate poll
	TimeSinceLastHeartbeat = 0.0f;
	
	UE_LOG(LogOpenClaw, Log, TEXT("Started polling for commands"));
}

void FOpenClawConnectionManager::StopPolling()
{
	bIsPolling = false;
	bPollInFlight = false;
	
	UE_LOG(LogOpenClaw, Log, TEXT("Stopped polling"));
}

void FOpenClawConnectionManager::Tick(float DeltaTime)
{
	// Handle reconnection attempts
	if (State == EOpenClawConnectionState::Reconnecting)
	{
		static float ReconnectTimer = 0.0f;
		ReconnectTimer += DeltaTime;
		
		if (ReconnectTimer >= 5.0f)
		{
			ReconnectTimer = 0.0f;
			SendRegister();
		}
		return;
	}
	
	if (!bIsPolling)
	{
		return;
	}
	
	TimeSinceLastPoll += DeltaTime;
	TimeSinceLastHeartbeat += DeltaTime;
	
	// Poll for commands
	if (TimeSinceLastPoll >= PollInterval && !bPollInFlight)
	{
		Poll();
		TimeSinceLastPoll = 0.0f;
	}
	
	// Send heartbeat
	if (TimeSinceLastHeartbeat >= HeartbeatInterval)
	{
		SendHeartbeat();
		TimeSinceLastHeartbeat = 0.0f;
	}
}

TStatId FOpenClawConnectionManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FOpenClawConnectionManager, STATGROUP_Tickables);
}

void FOpenClawConnectionManager::Poll()
{
	if (bPollInFlight || SessionId.IsEmpty())
	{
		return;
	}
	
	bPollInFlight = true;
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildUrl(FString::Printf(TEXT("/unreal/poll?sessionId=%s"), *SessionId)));
	Request->SetVerb(TEXT("GET"));
	Request->OnProcessRequestComplete().BindRaw(this, &FOpenClawConnectionManager::HandlePollResponse);
	Request->ProcessRequest();
}

void FOpenClawConnectionManager::HandlePollResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	bPollInFlight = false;
	
	if (!bSuccess || !Response.IsValid())
	{
		if (State == EOpenClawConnectionState::Connected)
		{
			UE_LOG(LogOpenClaw, Warning, TEXT("Lost connection to OpenClaw Gateway"));
			SetState(EOpenClawConnectionState::Reconnecting);
			// Try to re-register
			SendRegister();
		}
		return;
	}
	
	int32 ResponseCode = Response->GetResponseCode();
	
	if (ResponseCode == 200)
	{
		// Parse and process command
		FString ResponseBody = Response->GetContentAsString();
		
		if (!ResponseBody.IsEmpty())
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
			
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				ProcessCommand(JsonObject);
			}
		}
	}
	else if (ResponseCode == 204)
	{
		// No pending commands - this is normal
	}
	else if (ResponseCode == 404)
	{
		// Session not found, need to re-register
		UE_LOG(LogOpenClaw, Warning, TEXT("Session expired, re-registering..."));
		SetState(EOpenClawConnectionState::Reconnecting);
		SendRegister();
	}
	else
	{
		UE_LOG(LogOpenClaw, Warning, TEXT("Poll failed with code: %d"), ResponseCode);
	}
}

void FOpenClawConnectionManager::SendHeartbeat()
{
	if (SessionId.IsEmpty())
	{
		return;
	}
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildUrl(TEXT("/unreal/heartbeat")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("sessionId"), SessionId);
	
	FString BodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);
	
	Request->SetContentAsString(BodyString);
	Request->OnProcessRequestComplete().BindRaw(this, &FOpenClawConnectionManager::HandleHeartbeatResponse);
	Request->ProcessRequest();
}

void FOpenClawConnectionManager::HandleHeartbeatResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (bSuccess && Response.IsValid() && Response->GetResponseCode() == 200)
	{
		UE_LOG(LogOpenClaw, Verbose, TEXT("Heartbeat OK"));
	}
}

void FOpenClawConnectionManager::ProcessCommand(const TSharedPtr<FJsonObject>& Command)
{
	if (!Command.IsValid())
	{
		return;
	}
	
	FString ToolName = Command->GetStringField(TEXT("tool"));
	FString ToolCallId = Command->GetStringField(TEXT("toolCallId"));
	TSharedPtr<FJsonObject> Arguments = Command->GetObjectField(TEXT("arguments"));
	
	UE_LOG(LogOpenClaw, Log, TEXT("Received command: %s (id: %s)"), *ToolName, *ToolCallId);
	
	// Execute on game thread
	ExecuteToolOnGameThread(ToolName, ToolCallId, Arguments);
}

void FOpenClawConnectionManager::ExecuteToolOnGameThread(const FString& ToolName, const FString& ToolCallId, const TSharedPtr<FJsonObject>& Arguments)
{
	AsyncTask(ENamedThreads::GameThread, [this, ToolName, ToolCallId, Arguments]()
	{
		TSharedPtr<FJsonObject> Result = FOpenClawTools::ExecuteTool(ToolName, Arguments);
		SendToolResult(ToolCallId, Result);
	});
}

void FOpenClawConnectionManager::SendToolResult(const FString& ToolCallId, const TSharedPtr<FJsonObject>& Result)
{
	if (SessionId.IsEmpty())
	{
		return;
	}
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildUrl(TEXT("/unreal/result")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("sessionId"), SessionId);
	Body->SetStringField(TEXT("toolCallId"), ToolCallId);
	Body->SetObjectField(TEXT("result"), Result);
	
	FString BodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);
	
	Request->SetContentAsString(BodyString);
	Request->OnProcessRequestComplete().BindLambda([ToolCallId](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
	{
		if (bSuccess && Response.IsValid() && Response->GetResponseCode() == 200)
		{
			UE_LOG(LogOpenClaw, Log, TEXT("Result sent for: %s"), *ToolCallId);
		}
		else
		{
			UE_LOG(LogOpenClaw, Warning, TEXT("Failed to send result for: %s"), *ToolCallId);
		}
	});
	Request->ProcessRequest();
}

void FOpenClawConnectionManager::SetState(EOpenClawConnectionState NewState)
{
	if (State != NewState)
	{
		State = NewState;
		
		FString StateString;
		switch (State)
		{
			case EOpenClawConnectionState::Disconnected: StateString = TEXT("Disconnected"); break;
			case EOpenClawConnectionState::Connecting: StateString = TEXT("Connecting"); break;
			case EOpenClawConnectionState::Connected: StateString = TEXT("Connected"); break;
			case EOpenClawConnectionState::Reconnecting: StateString = TEXT("Reconnecting"); break;
			case EOpenClawConnectionState::Error: StateString = TEXT("Error"); break;
		}
		
		UE_LOG(LogOpenClaw, Log, TEXT("Connection state: %s"), *StateString);
		
		OnStateChanged.Broadcast(State);
	}
}

FString FOpenClawConnectionManager::BuildUrl(const FString& Endpoint) const
{
	return FString::Printf(TEXT("http://%s:%d%s"), *GatewayHost, GatewayPort, *Endpoint);
}
