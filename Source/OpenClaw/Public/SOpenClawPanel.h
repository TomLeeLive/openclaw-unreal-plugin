// Copyright OpenClaw. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "OpenClawConnectionManager.h"

/**
 * OpenClaw Editor Panel - Connection status, controls, and log
 */
class SOpenClawPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOpenClawPanel) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SOpenClawPanel();

private:
	/** Refresh displayed state */
	void RefreshStatus();

	/** Button handlers */
	FReply OnConnectClicked();
	FReply OnDisconnectClicked();
	FReply OnReconnectClicked();

	/** Get status color */
	FSlateColor GetStatusColor() const;
	FText GetStatusText() const;
	FText GetSessionText() const;
	FText GetGatewayText() const;
	FText GetToolCountText() const;
	FText GetUptimeText() const;
	FText GetMCPHostText() const;
	FText GetMCPPortText() const;

	/** State changed delegate */
	void OnConnectionStateChanged(EOpenClawConnectionState NewState);
	FDelegateHandle StateChangedHandle;

	/** Log */
	void AddLogEntry(const FString& Message);
	TSharedPtr<SVerticalBox> LogBox;
	TSharedPtr<SScrollBox> LogScrollBox;
	TArray<FString> LogEntries;
	static const int32 MaxLogEntries = 100;

	/** Timer */
	FDateTime ConnectedSince;
	bool bWasConnected = false;
};
