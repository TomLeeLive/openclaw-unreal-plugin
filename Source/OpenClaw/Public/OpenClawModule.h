// Copyright OpenClaw. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

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
	void RegisterMenuExtension();
	void UnregisterMenuExtension();
	
	FDelegateHandle ToolMenusHandle;
};
