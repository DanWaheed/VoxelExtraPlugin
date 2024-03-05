#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelObjectPinType.h"
#include "VoxelFunctionLibrary.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "FunctionLibrary/VoxelHeightmapFunctionLibrary.h"
#include "VoxelSampleHeightmapFromArray.generated.h"

// Given an array and an index and a value, sets the item in the array at that index
USTRUCT(Category = "Heightmap Texture", meta = (AdvancedDisplay = "Interpolation, BicubicSmoothness"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_SampleHeightmapFromArray : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelVector2DBuffer, Position, nullptr);
	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Heights, nullptr, ArrayPin);
	VOXEL_INPUT_PIN(float, Offset, 0.f);
	VOXEL_INPUT_PIN(float, ScaleZ, 2.f);
	VOXEL_INPUT_PIN(float, ScaleXY, 100.f);
	VOXEL_INPUT_PIN(EVoxelHeightmapInterpolationType, Interpolation, EVoxelHeightmapInterpolationType::Bicubic);
	VOXEL_INPUT_PIN(float, BicubicSmoothness, 1.f);
	VOXEL_OUTPUT_PIN(FVoxelFloatBuffer, Result);
};