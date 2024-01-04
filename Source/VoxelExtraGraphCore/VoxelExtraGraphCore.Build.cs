using System.IO;
using UnrealBuildTool;

public class VoxelExtraGraphCore : ModuleRules_Voxel
{
    public VoxelExtraGraphCore(ReadOnlyTargetRules Target) : base(Target)
	{
		//OptimizeCode = CodeOptimization.Never;
        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"VoxelCore",
				"VoxelGraphCore"
            }
		);
    }
}