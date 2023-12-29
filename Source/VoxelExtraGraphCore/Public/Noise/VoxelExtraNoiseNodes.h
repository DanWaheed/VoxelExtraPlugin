#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelExtraNoiseNodes.generated.h"

USTRUCT(Category = "Noise")
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_TrueDistanceCellularNoise3D : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector, Position, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Amplitude, 10000.f);
	VOXEL_TEMPLATE_INPUT_PIN(float, FeatureScale, 100000.f);
	VOXEL_TEMPLATE_INPUT_PIN(float, Jitter, 0.9f);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Value);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVector, CellPosition);

	virtual FString GenerateCode(FCode& Code) const override
	{
		Code.AddInclude("VoxelExtraNoiseNodesImpl.isph");
		return
			"{Value} = GetTrueDistanceCellularNoise3D({Seed}, {Position} / {FeatureScale}, {Jitter}, &{CellPosition}) * {Amplitude};"
			"{CellPosition} = {CellPosition} * {FeatureScale}";
	}
};