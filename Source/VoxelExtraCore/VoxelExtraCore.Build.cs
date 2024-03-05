using System.IO;
using UnrealBuildTool;

public class VoxelExtraCore : ModuleRules_Voxel
{
    public VoxelExtraCore(ReadOnlyTargetRules Target) : base(Target)
	{
        //OptimizeCode = CodeOptimization.Never;
        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "VoxelCore",
				"VoxelGraphCore"
            }
		);
    }
}