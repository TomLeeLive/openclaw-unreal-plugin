// Copyright OpenClaw. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"

/**
 * OpenClaw Tools
 * Implements all tool handlers for Unreal Engine
 */
class OPENCLAW_API FOpenClawTools
{
public:
	/** Execute a tool by name */
	static TSharedPtr<FJsonObject> ExecuteTool(const FString& ToolName, const TSharedPtr<FJsonObject>& Params);
	
	/** Get the total number of available tools */
	static int32 GetToolCount();
	
private:
	// Level tools
	static TSharedPtr<FJsonObject> Level_GetCurrent(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Level_List(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Level_Open(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Level_Save(const TSharedPtr<FJsonObject>& Params);
	
	// Actor tools
	static TSharedPtr<FJsonObject> Actor_Find(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Actor_GetAll(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Actor_Create(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Actor_Delete(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Actor_GetData(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Actor_SetProperty(const TSharedPtr<FJsonObject>& Params);
	
	// Transform tools
	static TSharedPtr<FJsonObject> Transform_GetPosition(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Transform_SetPosition(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Transform_GetRotation(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Transform_SetRotation(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Transform_GetScale(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Transform_SetScale(const TSharedPtr<FJsonObject>& Params);
	
	// Component tools
	static TSharedPtr<FJsonObject> Component_Get(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Component_Add(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Component_Remove(const TSharedPtr<FJsonObject>& Params);
	
	// Editor tools
	static TSharedPtr<FJsonObject> Editor_Play(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Editor_Stop(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Editor_Pause(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Editor_Resume(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Editor_GetState(const TSharedPtr<FJsonObject>& Params);
	
	// Debug tools
	static TSharedPtr<FJsonObject> Debug_Hierarchy(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Debug_Screenshot(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Debug_Log(const TSharedPtr<FJsonObject>& Params);
	
	// Input tools
	static TSharedPtr<FJsonObject> Input_SimulateKey(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Input_SimulateMouse(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Input_SimulateAxis(const TSharedPtr<FJsonObject>& Params);
	
	// Asset tools
	static TSharedPtr<FJsonObject> Asset_List(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Asset_Import(const TSharedPtr<FJsonObject>& Params);
	
	// Console tools
	static TSharedPtr<FJsonObject> Console_Execute(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Console_GetLogs(const TSharedPtr<FJsonObject>& Params);
	
	// Blueprint tools
	static TSharedPtr<FJsonObject> Blueprint_List(const TSharedPtr<FJsonObject>& Params);
	static TSharedPtr<FJsonObject> Blueprint_Open(const TSharedPtr<FJsonObject>& Params);
	
	// Helper functions
	static TSharedPtr<FJsonObject> MakeSuccessResult(const FString& Message);
	static TSharedPtr<FJsonObject> MakeErrorResult(const FString& Error);
	static TSharedPtr<FJsonObject> ActorToJson(AActor* Actor, bool bDetailed = false);
	static AActor* FindActorByName(const FString& Name);
	static UWorld* GetEditorWorld();
};
