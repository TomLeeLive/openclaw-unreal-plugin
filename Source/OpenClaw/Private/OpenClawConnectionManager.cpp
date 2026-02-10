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

DEFINE_LOG_CATEGORY(LogOpenClaw);

FOpenClawConnectionManager* FOpenClawConnectionManager::Instance = nullptr;

FOpenClawConnectionManager::FOpenClawConnectionManager()
	: GatewayHost(TEXT("127.0.0.1"))
	, GatewayPort(27742)
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
	UE_LOG(LogOpenClaw, Log, TEXT("Initializing connection manager..."));
	
	LoadConfig();
	
	if (bAutoConnect)
	{
		Connect();
	}
}

void FOpenClawConnectionManager::Shutdown()
{
	UE_LOG(LogOpenClaw, Log, TEXT("Shutting down connection manager..."));
	
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
		UE_LOG(LogOpenClaw, Log, TEXT("Using default config (no config file found)"));
	}
}

void FOpenClawConnectionManager::Connect()
{
	if (State == EOpenClawConnectionState::Connected || State == EOpenClawConnectionState::Connecting)
	{
		return;
	}
	
	SetState(EOpenClawConnectionState::Connecting);
	
	// Generate session ID
	SessionId = FString::Printf(TEXT("unreal_%lld_%s"), 
		FDateTime::Now().ToUnixTimestamp(),
		*FGuid::NewGuid().ToString().Left(8).ToLower());
	
	UE_LOG(LogOpenClaw, Log, TEXT("Connecting with session ID: %s"), *SessionId);
	
	StartPolling();
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
	
	UE_LOG(LogOpenClaw, Log, TEXT("Disconnected"));
}

void FOpenClawConnectionManager::StartPolling()
{
	bIsPolling = true;
	TimeSinceLastPoll = PollInterval; // Trigger immediate poll
	TimeSinceLastHeartbeat = 0.0f;
	
	UE_LOG(LogOpenClaw, Log, TEXT("Started polling"));
}

void FOpenClawConnectionManager::StopPolling()
{
	bIsPolling = false;
	bPollInFlight = false;
	
	UE_LOG(LogOpenClaw, Log, TEXT("Stopped polling"));
}

void FOpenClawConnectionManager::Tick(float DeltaTime)
{
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
	if (bPollInFlight)
	{
		return;
	}
	
	bPollInFlight = true;
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildUrl(TEXT("/poll")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	// Build request body
	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("sessionId"), SessionId);
	Body->SetStringField(TEXT("engine"), TEXT("unreal"));
	Body->SetStringField(TEXT("version"), TEXT("1.0.0"));
	
	// Add pending results
	{
		FScopeLock Lock(&PendingResultsLock);
		if (PendingResults.Num() > 0)
		{
			TArray<TSharedPtr<FJsonValue>> ResultsArray;
			for (const auto& Pair : PendingResults)
			{
				TSharedPtr<FJsonObject> ResultObj = MakeShareable(new FJsonObject());
				ResultObj->SetStringField(TEXT("toolCallId"), Pair.Key);
				ResultObj->SetObjectField(TEXT("result"), Pair.Value);
				ResultsArray.Add(MakeShareable(new FJsonValueObject(ResultObj)));
			}
			Body->SetArrayField(TEXT("results"), ResultsArray);
			PendingResults.Empty();
		}
	}
	
	FString BodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);
	
	Request->SetContentAsString(BodyString);
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
			SetState(EOpenClawConnectionState::Reconnecting);
		}
		return;
	}
	
	int32 ResponseCode = Response->GetResponseCode();
	
	if (ResponseCode == 200)
	{
		if (State != EOpenClawConnectionState::Connected)
		{
			SetState(EOpenClawConnectionState::Connected);
		}
		
		// Parse response
		FString ResponseBody = Response->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
		
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			// Process commands
			if (JsonObject->HasField(TEXT("commands")))
			{
				const TArray<TSharedPtr<FJsonValue>>& Commands = JsonObject->GetArrayField(TEXT("commands"));
				for (const auto& CommandValue : Commands)
				{
					if (CommandValue->Type == EJson::Object)
					{
						ProcessCommand(CommandValue->AsObject());
					}
				}
			}
		}
	}
	else if (ResponseCode == 404)
	{
		// Gateway not found, keep trying
		if (State == EOpenClawConnectionState::Connected)
		{
			SetState(EOpenClawConnectionState::Reconnecting);
		}
	}
	else
	{
		UE_LOG(LogOpenClaw, Warning, TEXT("Poll failed with code: %d"), ResponseCode);
	}
}

void FOpenClawConnectionManager::SendHeartbeat()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildUrl(TEXT("/heartbeat")));
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
	TSharedPtr<FJsonObject> Params = Command->GetObjectField(TEXT("params"));
	
	UE_LOG(LogOpenClaw, Log, TEXT("Received command: %s (id: %s)"), *ToolName, *ToolCallId);
	
	// Execute on game thread
	ExecuteToolOnGameThread(ToolName, ToolCallId, Params);
}

void FOpenClawConnectionManager::ExecuteToolOnGameThread(const FString& ToolName, const FString& ToolCallId, const TSharedPtr<FJsonObject>& Params)
{
	AsyncTask(ENamedThreads::GameThread, [this, ToolName, ToolCallId, Params]()
	{
		TSharedPtr<FJsonObject> Result = FOpenClawTools::ExecuteTool(ToolName, Params);
		SendToolResult(ToolCallId, Result);
	});
}

void FOpenClawConnectionManager::SendToolResult(const FString& ToolCallId, const TSharedPtr<FJsonObject>& Result)
{
	FScopeLock Lock(&PendingResultsLock);
	PendingResults.Add(TPair<FString, TSharedPtr<FJsonObject>>(ToolCallId, Result));
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
		
		UE_LOG(LogOpenClaw, Log, TEXT("State changed to: %s"), *StateString);
		
		OnStateChanged.Broadcast(State);
	}
}

FString FOpenClawConnectionManager::BuildUrl(const FString& Endpoint) const
{
	return FString::Printf(TEXT("http://%s:%d/api/plugin%s"), *GatewayHost, GatewayPort, *Endpoint);
}
