// Copyright OpenClaw. All Rights Reserved.

using UnrealBuildTool;

public class OpenClaw : ModuleRules
{
	public OpenClaw(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
		);
				
		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);
			
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"HTTP",
				"HTTPServer",
				"Json",
				"JsonUtilities",
				"Sockets",
				"Networking"
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"UnrealEd",
				"LevelEditor",
				"EditorStyle",
				"Projects",
				"ToolMenus",
				"EditorSubsystem",
				"ImageWrapper",
				"WorkspaceMenuStructure"
			}
		);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
