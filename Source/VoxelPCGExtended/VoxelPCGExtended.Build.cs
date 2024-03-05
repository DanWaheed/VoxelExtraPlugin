using System.IO;
using UnrealBuildTool;

public class VoxelPCGExtended : ModuleRules_Voxel
{
    public VoxelPCGExtended(ReadOnlyTargetRules Target) : base(Target)
	{
        //OptimizeCode = CodeOptimization.Never;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
			new[]
			{
				"VoxelCore",
				"VoxelGraphCore",
                "PCG"
            }
		);

        PrivateIncludePaths.AddRange(
            new[] {
                Path.GetFullPath(Path.Combine(EngineDirectory, @"Plugins\Experimental\PCG\Source\PCG\Private")),
            });
    }
}