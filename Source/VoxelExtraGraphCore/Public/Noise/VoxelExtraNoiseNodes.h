#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelExtraNoiseNodes.generated.h"

USTRUCT(Category = "Noise", meta = (DisplayName = "2D IQ Noise"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_IQNoise2D : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, Position, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Amplitude, 10000.f);
	VOXEL_TEMPLATE_INPUT_PIN(float, FeatureScale, 100000.f);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Value);

	virtual FString GenerateCode(FCode& Code) const override
	{
		Code.AddInclude("VoxelExtraNoiseNodesImpl.isph");
		return
			"{Value} = VoxelExtraNoiseNodes_Get2DIQ({Seed}, {Position} / {FeatureScale}) * {Amplitude}";
	}
};

USTRUCT(Category = "Noise", meta = (DisplayName = "2D Simplex Noise"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_SimplexNoise2D : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, Position, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Amplitude, 10000.f);
	VOXEL_TEMPLATE_INPUT_PIN(float, FeatureScale, 100000.f);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Value);

	virtual FString GenerateCode(FCode& Code) const override
	{
		Code.AddInclude("VoxelExtraNoiseNodesImpl.isph");
		return
			"{Value} = VoxelExtraNoiseNodes_SimplexNoise2D({Seed}, {Position} / {FeatureScale}) * {Amplitude}";
	}
};

USTRUCT(Category = "Noise", meta = (DisplayName = "2D Simplex Noise Fractal"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_SimplexNoiseFractal2D : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, Position, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Amplitude, 10000.f);
	VOXEL_TEMPLATE_INPUT_PIN(float, FeatureScale, 100000.f);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, Octaves, 9);
	VOXEL_TEMPLATE_INPUT_PIN(float, Lacunarity, 2.0f);
	VOXEL_TEMPLATE_INPUT_PIN(float, Persistence, 0.5f);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Value);

	virtual FString GenerateCode(FCode& Code) const override
	{
		Code.AddInclude("VoxelExtraNoiseNodesImpl.isph");
		return
			"{Value} = VoxelExtraNoiseNodes_SimplexFractal2D({Seed}, {Position} / {FeatureScale}, {Octaves}, {Lacunarity}, {Persistence}) * {Amplitude}";
	}
};