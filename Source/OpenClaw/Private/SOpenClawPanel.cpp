// Copyright OpenClaw. All Rights Reserved.

#include "SOpenClawPanel.h"
#include "OpenClawConnectionManager.h"
#include "OpenClawHttpServer.h"
#include "OpenClawTools.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "OpenClawPanel"

void SOpenClawPanel::Construct(const FArguments& InArgs)
{
	// Subscribe to state changes
	StateChangedHandle = FOpenClawConnectionManager::Get().OnStateChanged.AddRaw(this, &SOpenClawPanel::OnConnectionStateChanged);

	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		.Padding(12.0f)
		[
			SNew(SVerticalBox)

			// Header
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 0, 0, 8)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("\U0001F99E")))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(8, 0, 0, 0)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Title", "OpenClaw Unreal Plugin"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
				]
			]

			// Separator
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 4)
			[
				SNew(SSeparator)
			]

			// Status
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("StatusLabel", "Status: "))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(this, &SOpenClawPanel::GetStatusText)
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
					.ColorAndOpacity(this, &SOpenClawPanel::GetStatusColor)
				]
			]

			// Session ID
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 2)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SessionLabel", "Session: "))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(STextBlock)
					.Text(this, &SOpenClawPanel::GetSessionText)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
			]

			// Gateway
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 2)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("GatewayLabel", "Gateway: "))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(this, &SOpenClawPanel::GetGatewayText)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
			]

			// Tools count
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 2)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ToolsLabel", "Tools: "))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(this, &SOpenClawPanel::GetToolCountText)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
			]

			// Uptime
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 2)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("UptimeLabel", "Uptime: "))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(this, &SOpenClawPanel::GetUptimeText)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
			]

			// MCP Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 12, 0, 4)
			[
				SNew(SSeparator)
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 4)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("MCPHeader", "MCP Connection"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("MCPHostLabel", "Host: "))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(this, &SOpenClawPanel::GetMCPHostText)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 2)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("MCPPortLabel", "Port: "))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(this, &SOpenClawPanel::GetMCPPortText)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 2)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("MCPProtocolLabel", "Protocol: "))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("MCPProtocol", "HTTP Polling (SSE)"))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
				]
			]

			// Buttons
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 12, 0, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0, 0, 8, 0)
				[
					SNew(SButton)
					.Text(LOCTEXT("Connect", "Connect"))
					.OnClicked(this, &SOpenClawPanel::OnConnectClicked)
					.IsEnabled_Lambda([]() {
						return FOpenClawConnectionManager::Get().GetState() == EOpenClawConnectionState::Disconnected
							|| FOpenClawConnectionManager::Get().GetState() == EOpenClawConnectionState::Error;
					})
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0, 0, 8, 0)
				[
					SNew(SButton)
					.Text(LOCTEXT("Disconnect", "Disconnect"))
					.OnClicked(this, &SOpenClawPanel::OnDisconnectClicked)
					.IsEnabled_Lambda([]() {
						return FOpenClawConnectionManager::Get().IsConnected();
					})
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("Reconnect", "Reconnect"))
					.OnClicked(this, &SOpenClawPanel::OnReconnectClicked)
				]
			]

			// Log separator
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 8, 0, 4)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("LogHeader", "Log"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 2)
			[
				SNew(SSeparator)
			]

			// Log entries
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(0, 4)
			[
				SAssignNew(LogScrollBox, SScrollBox)
				+ SScrollBox::Slot()
				[
					SAssignNew(LogBox, SVerticalBox)
				]
			]
		]
	];

	// Initial log entry
	AddLogEntry(TEXT("OpenClaw panel initialized"));
	if (FOpenClawConnectionManager::Get().IsConnected())
	{
		AddLogEntry(TEXT("Already connected to gateway"));
		ConnectedSince = FDateTime::Now();
		bWasConnected = true;
	}
}

SOpenClawPanel::~SOpenClawPanel()
{
	if (StateChangedHandle.IsValid())
	{
		FOpenClawConnectionManager::Get().OnStateChanged.Remove(StateChangedHandle);
	}
}

FReply SOpenClawPanel::OnConnectClicked()
{
	AddLogEntry(TEXT("Connecting..."));
	FOpenClawConnectionManager::Get().Connect();
	return FReply::Handled();
}

FReply SOpenClawPanel::OnDisconnectClicked()
{
	AddLogEntry(TEXT("Disconnecting..."));
	FOpenClawConnectionManager::Get().Disconnect();
	return FReply::Handled();
}

FReply SOpenClawPanel::OnReconnectClicked()
{
	AddLogEntry(TEXT("Reconnecting..."));
	FOpenClawConnectionManager::Get().Disconnect();
	FOpenClawConnectionManager::Get().Connect();
	return FReply::Handled();
}

FSlateColor SOpenClawPanel::GetStatusColor() const
{
	switch (FOpenClawConnectionManager::Get().GetState())
	{
	case EOpenClawConnectionState::Connected:
		return FSlateColor(FLinearColor::Green);
	case EOpenClawConnectionState::Connecting:
	case EOpenClawConnectionState::Reconnecting:
		return FSlateColor(FLinearColor::Yellow);
	case EOpenClawConnectionState::Error:
		return FSlateColor(FLinearColor::Red);
	default:
		return FSlateColor(FLinearColor::Gray);
	}
}

FText SOpenClawPanel::GetStatusText() const
{
	switch (FOpenClawConnectionManager::Get().GetState())
	{
	case EOpenClawConnectionState::Connected:
		return FText::FromString(TEXT("\u25CF Connected"));
	case EOpenClawConnectionState::Connecting:
		return FText::FromString(TEXT("\u25CB Connecting..."));
	case EOpenClawConnectionState::Reconnecting:
		return FText::FromString(TEXT("\u25CB Reconnecting..."));
	case EOpenClawConnectionState::Error:
		return FText::FromString(TEXT("\u25CF Error"));
	default:
		return FText::FromString(TEXT("\u25CB Disconnected"));
	}
}

FText SOpenClawPanel::GetSessionText() const
{
	FString SessionId = FOpenClawConnectionManager::Get().GetSessionId();
	return SessionId.IsEmpty() ? FText::FromString(TEXT("—")) : FText::FromString(SessionId);
}

FText SOpenClawPanel::GetGatewayText() const
{
	if (FOpenClawConnectionManager::Get().IsConnected())
	{
		return FText::FromString(TEXT("localhost:42424"));
	}
	return FText::FromString(TEXT("—"));
}

FText SOpenClawPanel::GetToolCountText() const
{
	if (FOpenClawConnectionManager::Get().IsConnected())
	{
		return FText::FromString(FString::Printf(TEXT("%d"), FOpenClawTools::GetToolCount()));
	}
	return FText::FromString(TEXT("—"));
}

FText SOpenClawPanel::GetUptimeText() const
{
	if (!bWasConnected || FOpenClawConnectionManager::Get().GetState() != EOpenClawConnectionState::Connected)
	{
		return FText::FromString(TEXT("—"));
	}

	FTimespan Uptime = FDateTime::Now() - ConnectedSince;
	int32 Hours = Uptime.GetHours();
	int32 Minutes = Uptime.GetMinutes();
	int32 Seconds = Uptime.GetSeconds();

	if (Hours > 0)
	{
		return FText::FromString(FString::Printf(TEXT("%dh %dm %ds"), Hours, Minutes, Seconds));
	}
	else if (Minutes > 0)
	{
		return FText::FromString(FString::Printf(TEXT("%dm %ds"), Minutes, Seconds));
	}
	return FText::FromString(FString::Printf(TEXT("%ds"), Seconds));
}

FText SOpenClawPanel::GetMCPHostText() const
{
	if (FOpenClawConnectionManager::Get().IsConnected())
	{
		return FText::FromString(TEXT("localhost"));
	}
	return FText::FromString(TEXT("—"));
}

FText SOpenClawPanel::GetMCPPortText() const
{
	if (FOpenClawHttpServer::Get().IsRunning())
	{
		return FText::FromString(FString::Printf(TEXT("%d (Direct)"), FOpenClawHttpServer::Get().GetPort()));
	}
	if (FOpenClawConnectionManager::Get().IsConnected())
	{
		return FText::FromString(FString::Printf(TEXT("%d (Gateway)"), 18789));
	}
	return FText::FromString(TEXT("—"));
}

void SOpenClawPanel::OnConnectionStateChanged(EOpenClawConnectionState NewState)
{
	switch (NewState)
	{
	case EOpenClawConnectionState::Connected:
		AddLogEntry(TEXT("Connected to gateway"));
		ConnectedSince = FDateTime::Now();
		bWasConnected = true;
		break;
	case EOpenClawConnectionState::Disconnected:
		AddLogEntry(TEXT("Disconnected"));
		bWasConnected = false;
		break;
	case EOpenClawConnectionState::Error:
		AddLogEntry(TEXT("Connection error"));
		bWasConnected = false;
		break;
	case EOpenClawConnectionState::Reconnecting:
		AddLogEntry(TEXT("Reconnecting..."));
		break;
	default:
		break;
	}
}

void SOpenClawPanel::AddLogEntry(const FString& Message)
{
	FString Timestamp = FDateTime::Now().ToString(TEXT("%H:%M:%S"));
	FString Entry = FString::Printf(TEXT("[%s] %s"), *Timestamp, *Message);

	LogEntries.Add(Entry);
	if (LogEntries.Num() > MaxLogEntries)
	{
		LogEntries.RemoveAt(0);
	}

	if (LogBox.IsValid())
	{
		LogBox->AddSlot()
		.AutoHeight()
		.Padding(0, 1)
		[
			SNew(STextBlock)
			.Text(FText::FromString(Entry))
			.Font(FCoreStyle::GetDefaultFontStyle("Mono", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)))
		];

		// Auto-scroll to bottom
		if (LogScrollBox.IsValid())
		{
			LogScrollBox->ScrollToEnd();
		}
	}
}

#undef LOCTEXT_NAMESPACE
