// Copyright OpenClaw. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpRouteHandle.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"

/**
 * Embedded HTTP server for MCP Direct mode.
 * Listens on port 27184 and accepts tool execution requests
 * from Claude Code / Cursor via MCP~/index.js bridge.
 */
class OPENCLAW_API FOpenClawHttpServer
{
public:
	static FOpenClawHttpServer& Get();
	
	/** Start HTTP server on given port */
	void Start(int32 Port = 27184);
	
	/** Stop HTTP server */
	void Stop();
	
	/** Check if server is running */
	bool IsRunning() const { return bIsRunning; }
	
	/** Get port */
	int32 GetPort() const { return ServerPort; }

private:
	FOpenClawHttpServer() = default;
	~FOpenClawHttpServer();
	
	FOpenClawHttpServer(const FOpenClawHttpServer&) = delete;
	FOpenClawHttpServer& operator=(const FOpenClawHttpServer&) = delete;
	
	/** Handle POST /tool */
	bool HandleToolRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	
	/** Handle GET /status */
	bool HandleStatusRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	
	/** Send JSON response */
	void SendJsonResponse(const FHttpResultCallback& OnComplete, int32 Code, const TSharedPtr<FJsonObject>& Json);
	
	bool bIsRunning = false;
	int32 ServerPort = 27184;
	FHttpRouteHandle ToolRouteHandle;
	FHttpRouteHandle StatusRouteHandle;
	
	static FOpenClawHttpServer* Instance;
};
