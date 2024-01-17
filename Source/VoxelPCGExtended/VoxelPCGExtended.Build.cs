using System.IO;
using UnrealBuildTool;

public class VoxelPCGExtended : ModuleRules_Voxel
{
    public VoxelPCGExtended(ReadOnlyTargetRules Target) : base(Target)
	{
        //OptimizeCode = CodeOptimization.Never;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"VoxelCore",
				"VoxelGraphCore"
            }
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "NavigationSystem",
                "StaticMeshDescription",
                "MeshDescription",
                "Engine",
				"PCG",
                "PCGGeometryScriptInterop",
                "UnrealEd"
            }
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects", // So that we can use the IPluginManager, required for icons
                "DetailCustomizations",
                "PCG"
            }
		);

		PrivateIncludePathModuleNames.Add("PCG");

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.GetFullPath(Path.Combine(EngineDirectory, "Plugins\\Experimental\\PCG\\Source\\PCG\\Private")),
            });
    }
}