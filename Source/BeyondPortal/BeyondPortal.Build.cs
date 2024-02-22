// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BeyondPortal : ModuleRules
{
	public BeyondPortal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "BeyondPortal" });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });
	}
}
