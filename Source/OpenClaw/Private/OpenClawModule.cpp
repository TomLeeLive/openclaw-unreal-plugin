// Copyright OpenClaw. All Rights Reserved.

#include "OpenClawModule.h"
#include "OpenClawConnectionManager.h"
#include "OpenClawTools.h"
#include "ToolMenus.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "FOpenClawModule"

void FOpenClawModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("[OpenClaw] 🦞 Plugin starting..."));
	
	// Initialize connection manager
	FOpenClawConnectionManager::Get().Initialize();
	
	// Register menu extension
	RegisterMenuExtension();
	
	UE_LOG(LogTemp, Log, TEXT("[OpenClaw] 🦞 Plugin started successfully"));
}

void FOpenClawModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("[OpenClaw] 🦞 Plugin shutting down..."));
	
	UnregisterMenuExtension();
	
	// Shutdown connection manager
	FOpenClawConnectionManager::Get().Shutdown();
	
	UE_LOG(LogTemp, Log, TEXT("[OpenClaw] 🦞 Plugin shutdown complete"));
}

FOpenClawModule& FOpenClawModule::Get()
{
	return FModuleManager::LoadModuleChecked<FOpenClawModule>("OpenClaw");
}

bool FOpenClawModule::IsAvailable()
{
	return FModuleManager::Get().IsModuleLoaded("OpenClaw");
}

void FOpenClawModule::RegisterMenuExtension()
{
	if (!UToolMenus::IsToolMenuUIEnabled())
	{
		return;
	}
	
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus)
	{
		return;
	}
	
	// Add to Window menu
	UToolMenu* WindowMenu = ToolMenus->ExtendMenu("LevelEditor.MainMenu.Window");
	if (WindowMenu)
	{
		FToolMenuSection& Section = WindowMenu->FindOrAddSection("OpenClaw");
		Section.AddMenuEntry(
			"OpenClawStatus",
			LOCTEXT("OpenClawStatusLabel", "OpenClaw Status"),
			LOCTEXT("OpenClawStatusTooltip", "Show OpenClaw connection status"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([]()
			{
				FOpenClawConnectionManager& Manager = FOpenClawConnectionManager::Get();
				FString Status = Manager.IsConnected() 
					? TEXT("Connected to OpenClaw Gateway")
					: TEXT("Disconnected from OpenClaw Gateway");
				
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Status));
			}))
		);
	}
}

void FOpenClawModule::UnregisterMenuExtension()
{
	if (UToolMenus* ToolMenus = UToolMenus::Get())
	{
		ToolMenus->RemoveMenu("LevelEditor.MainMenu.Window");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenClawModule, OpenClaw)
