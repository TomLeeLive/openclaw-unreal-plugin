// Copyright OpenClaw. All Rights Reserved.

#include "OpenClawModule.h"
#include "OpenClawConnectionManager.h"
#include "OpenClawTools.h"
#include "SOpenClawPanel.h"
#include "ToolMenus.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#define LOCTEXT_NAMESPACE "FOpenClawModule"

static const FName OpenClawTabName("OpenClawPanel");

void FOpenClawModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("[OpenClaw] 🦞 Plugin starting..."));
	
	// Initialize connection manager
	FOpenClawConnectionManager::Get().Initialize();
	
	// Register tab spawner
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(OpenClawTabName,
		FOnSpawnTab::CreateRaw(this, &FOpenClawModule::SpawnOpenClawTab))
		.SetDisplayName(LOCTEXT("TabTitle", "OpenClaw"))
		.SetTooltipText(LOCTEXT("TabTooltip", "OpenClaw AI connection status and controls"))
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
	
	// Register menu extension
	RegisterMenuExtension();
	
	UE_LOG(LogTemp, Log, TEXT("[OpenClaw] 🦞 Plugin started successfully"));
}

void FOpenClawModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("[OpenClaw] 🦞 Plugin shutting down..."));
	
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(OpenClawTabName);
	
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

TSharedRef<SDockTab> FOpenClawModule::SpawnOpenClawTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(LOCTEXT("TabLabel", "OpenClaw"))
		[
			SNew(SOpenClawPanel)
		];
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
		Section.Label = LOCTEXT("OpenClawMenuSection", "OpenClaw");
		
		Section.AddMenuEntry(
			"OpenClawPanel",
			LOCTEXT("OpenClawPanelLabel", "OpenClaw Panel"),
			LOCTEXT("OpenClawPanelTooltip", "Open the OpenClaw connection panel"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([]()
			{
				FGlobalTabmanager::Get()->TryInvokeTab(OpenClawTabName);
			}))
		);
	}
}

void FOpenClawModule::UnregisterMenuExtension()
{
	// ToolMenus cleanup is handled automatically
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenClawModule, OpenClaw)
