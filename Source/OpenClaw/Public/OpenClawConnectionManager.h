// Copyright OpenClaw. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "Tickable.h"
#include "Dom/JsonObject.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOpenClaw, Log, All);

/**
 * Connection state enum
 */
enum class EOpenClawConnectionState : uint8
{
	Disconnected,
	Connecting,
	Connected,
	Reconnecting,
	Error
};

/**
 * OpenClaw Connection Manager
 * Handles HTTP polling communication with OpenClaw Gateway
 */
class OPENCLAW_API FOpenClawConnectionManager : public FTickableGameObject
{
public:
	/** Get singleton instance */
	static FOpenClawConnectionManager& Get();
	
	/** Initialize connection */
	void Initialize();
	
	/** Shutdown connection */
	void Shutdown();
	
	/** Connect to gateway */
	void Connect();
	
	/** Disconnect from gateway */
	void Disconnect();
	
	/** Check if connected */
	bool IsConnected() const { return State == EOpenClawConnectionState::Connected; }
	
	/** Get current state */
	EOpenClawConnectionState GetState() const { return State; }
	
	/** Get session ID */
	FString GetSessionId() const { return SessionId; }
	
	/** Send tool result */
	void SendToolResult(const FString& ToolCallId, const TSharedPtr<FJsonObject>& Result);
	
	// FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return true; }
	virtual bool IsTickableInEditor() const override { return true; }
	
	// Events
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateChanged, EOpenClawConnectionState);
	FOnStateChanged OnStateChanged;
	
private:
	FOpenClawConnectionManager();
	~FOpenClawConnectionManager();
	
	// Non-copyable
	FOpenClawConnectionManager(const FOpenClawConnectionManager&) = delete;
	FOpenClawConnectionManager& operator=(const FOpenClawConnectionManager&) = delete;
	
	/** Load config from file */
	void LoadConfig();
	
	/** Start polling loop */
	void StartPolling();
	
	/** Stop polling loop */
	void StopPolling();
	
	/** Poll for commands */
	void Poll();
	
	/** Send heartbeat */
	void SendHeartbeat();
	
	/** Handle poll response */
	void HandlePollResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	
	/** Handle heartbeat response */
	void HandleHeartbeatResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	
	/** Process received command */
	void ProcessCommand(const TSharedPtr<FJsonObject>& Command);
	
	/** Execute tool on game thread */
	void ExecuteToolOnGameThread(const FString& ToolName, const FString& ToolCallId, const TSharedPtr<FJsonObject>& Params);
	
	/** Set connection state */
	void SetState(EOpenClawConnectionState NewState);
	
	/** Build URL */
	FString BuildUrl(const FString& Endpoint) const;
	
	// State
	EOpenClawConnectionState State = EOpenClawConnectionState::Disconnected;
	FString SessionId;
	FString LastError;
	
	// Config
	FString GatewayHost;
	int32 GatewayPort;
	FString PluginSecret;
	bool bAutoConnect;
	
	// Polling
	bool bIsPolling = false;
	float PollInterval = 0.5f;
	float HeartbeatInterval = 30.0f;
	float TimeSinceLastPoll = 0.0f;
	float TimeSinceLastHeartbeat = 0.0f;
	bool bPollInFlight = false;
	
	// Pending results
	TArray<TPair<FString, TSharedPtr<FJsonObject>>> PendingResults;
	FCriticalSection PendingResultsLock;
	
	// Singleton
	static FOpenClawConnectionManager* Instance;
};
