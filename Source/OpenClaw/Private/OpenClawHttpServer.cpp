// Copyright OpenClaw. All Rights Reserved.

#include "OpenClawHttpServer.h"
#include "OpenClawConnectionManager.h"
#include "OpenClawTools.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Async/Async.h"

FOpenClawHttpServer* FOpenClawHttpServer::Instance = nullptr;

FOpenClawHttpServer& FOpenClawHttpServer::Get()
{
	if (!Instance)
	{
		Instance = new FOpenClawHttpServer();
	}
	return *Instance;
}

FOpenClawHttpServer::~FOpenClawHttpServer()
{
	Stop();
}

void FOpenClawHttpServer::Start(int32 Port)
{
	if (bIsRunning)
	{
		UE_LOG(LogOpenClaw, Warning, TEXT("HTTP server already running on port %d"), ServerPort);
		return;
	}
	
	ServerPort = Port;
	
	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> Router = HttpServerModule.GetHttpRouter(ServerPort);
	
	if (!Router.IsValid())
	{
		UE_LOG(LogOpenClaw, Error, TEXT("Failed to get HTTP router for port %d"), ServerPort);
		return;
	}
	
	// POST /tool — execute a tool
	ToolRouteHandle = Router->BindRoute(
		FHttpPath(TEXT("/tool")),
		EHttpServerRequestVerbs::VERB_POST,
		FHttpRequestHandler::CreateRaw(this, &FOpenClawHttpServer::HandleToolRequest)
	);
	
	// GET /status — server status
	StatusRouteHandle = Router->BindRoute(
		FHttpPath(TEXT("/status")),
		EHttpServerRequestVerbs::VERB_GET,
		FHttpRequestHandler::CreateRaw(this, &FOpenClawHttpServer::HandleStatusRequest)
	);
	
	HttpServerModule.StartAllListeners();
	
	bIsRunning = true;
	UE_LOG(LogOpenClaw, Log, TEXT("MCP Direct HTTP server started on port %d"), ServerPort);
}

void FOpenClawHttpServer::Stop()
{
	if (!bIsRunning)
	{
		return;
	}
	
	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> Router = HttpServerModule.GetHttpRouter(ServerPort);
	
	if (Router.IsValid())
	{
		Router->UnbindRoute(ToolRouteHandle);
		Router->UnbindRoute(StatusRouteHandle);
	}
	
	HttpServerModule.StopAllListeners();
	
	bIsRunning = false;
	UE_LOG(LogOpenClaw, Log, TEXT("MCP Direct HTTP server stopped"));
}

bool FOpenClawHttpServer::HandleToolRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	// Parse body
	FString BodyStr;
	const TArray<uint8>& Body = Request.Body;
	if (Body.Num() > 0)
	{
		FUTF8ToTCHAR Converter(reinterpret_cast<const ANSICHAR*>(Body.GetData()), Body.Num());
		BodyStr = FString(Converter.Length(), Converter.Get());
	}
	
	TSharedPtr<FJsonObject> JsonBody;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BodyStr);
	if (!FJsonSerializer::Deserialize(Reader, JsonBody) || !JsonBody.IsValid())
	{
		TSharedPtr<FJsonObject> Error = MakeShareable(new FJsonObject());
		Error->SetBoolField(TEXT("success"), false);
		Error->SetStringField(TEXT("error"), TEXT("Invalid JSON body"));
		SendJsonResponse(OnComplete, 400, Error);
		return true;
	}
	
	FString ToolName = JsonBody->GetStringField(TEXT("tool"));
	TSharedPtr<FJsonObject> Arguments = JsonBody->GetObjectField(TEXT("arguments"));
	
	if (ToolName.IsEmpty())
	{
		TSharedPtr<FJsonObject> Error = MakeShareable(new FJsonObject());
		Error->SetBoolField(TEXT("success"), false);
		Error->SetStringField(TEXT("error"), TEXT("Missing 'tool' field"));
		SendJsonResponse(OnComplete, 400, Error);
		return true;
	}
	
	if (!Arguments.IsValid())
	{
		Arguments = MakeShareable(new FJsonObject());
	}
	
	UE_LOG(LogOpenClaw, Log, TEXT("[MCP Direct] Executing tool: %s"), *ToolName);
	
	// Execute on game thread and respond
	AsyncTask(ENamedThreads::GameThread, [this, ToolName, Arguments, OnComplete]()
	{
		TSharedPtr<FJsonObject> Result = FOpenClawTools::ExecuteTool(ToolName, Arguments);
		
		TSharedPtr<FJsonObject> Response = MakeShareable(new FJsonObject());
		Response->SetBoolField(TEXT("success"), true);
		Response->SetObjectField(TEXT("result"), Result);
		
		SendJsonResponse(OnComplete, 200, Response);
	});
	
	return true;
}

bool FOpenClawHttpServer::HandleStatusRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	TSharedPtr<FJsonObject> Status = MakeShareable(new FJsonObject());
	Status->SetBoolField(TEXT("success"), true);
	Status->SetStringField(TEXT("plugin"), TEXT("OpenClaw Unreal Plugin"));
	Status->SetStringField(TEXT("version"), TEXT("1.3.0"));
	Status->SetBoolField(TEXT("mcpDirect"), true);
	Status->SetNumberField(TEXT("port"), ServerPort);
	Status->SetBoolField(TEXT("gatewayConnected"), FOpenClawConnectionManager::Get().IsConnected());
	
	SendJsonResponse(OnComplete, 200, Status);
	return true;
}

void FOpenClawHttpServer::SendJsonResponse(const FHttpResultCallback& OnComplete, int32 Code, const TSharedPtr<FJsonObject>& Json)
{
	FString JsonStr;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStr);
	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);
	
	auto Response = FHttpServerResponse::Create(JsonStr, TEXT("application/json"));
	OnComplete(MoveTemp(Response));
}
