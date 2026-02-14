// Copyright OpenClaw. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"

class FOpenClawModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** Get singleton instance */
	static FOpenClawModule& Get();
	
	/** Check if module is loaded */
	static bool IsAvailable();

private:
	/** Spawn the OpenClaw panel tab */
	TSharedRef<SDockTab> SpawnOpenClawTab(const FSpawnTabArgs& SpawnTabArgs);

	void RegisterMenuExtension();
	void UnregisterMenuExtension();
	
	FDelegateHandle ToolMenusHandle;
};
