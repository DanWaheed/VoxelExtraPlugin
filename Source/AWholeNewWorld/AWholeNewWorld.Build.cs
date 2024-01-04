// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class AWholeNewWorld : ModuleRules
{
	public AWholeNewWorld(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore"
        });

        PublicDependencyModuleNames.AddRange(new string[] { 
			"VoxelCore",
			"VoxelGraphCore",
			"VoxelExtraGraphCore"
		});

		if (Target.bBuildEditor)
		{
            PublicDependencyModuleNames.AddRange(new string[] {
                "VoxelBlueprint",
                "VoxelCoreEditor",
                "VoxelGraphEditor",
                "VoxelContentEditor",
                "VoxelBlueprintEditor",
                "VoxelExampleEditor"
            });
        }

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
