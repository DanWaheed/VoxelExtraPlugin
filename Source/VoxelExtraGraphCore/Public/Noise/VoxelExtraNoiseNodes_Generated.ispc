// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeHelpers.isph"
#include "VoxelExtraNoiseNodesImpl.isph"

export void VoxelNode_VoxelNode_IQNoise2D(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float2 Position = MakeFloat2(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index));
		const float Amplitude = LoadFloat(Buffers[2], Index);
		const float FeatureScale = LoadFloat(Buffers[3], Index);
		const int32 Seed = LoadInt32(Buffers[4], Index);
		float Value;

		Value = VoxelExtraNoiseNodes_Get2DIQ(Seed, Position / FeatureScale) * Amplitude;

		StoreFloat(Buffers[5], Index, Value);
	}
}

export void VoxelNode_VoxelNode_SimplexNoise2D(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float2 Position = MakeFloat2(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index));
		const float Amplitude = LoadFloat(Buffers[2], Index);
		const float FeatureScale = LoadFloat(Buffers[3], Index);
		const int32 Seed = LoadInt32(Buffers[4], Index);
		float Value;

		Value = VoxelExtraNoiseNodes_SimplexNoise2D(Seed, Position / FeatureScale) * Amplitude;

		StoreFloat(Buffers[5], Index, Value);
	}
}

export void VoxelNode_VoxelNode_SimplexNoiseFractal2D(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float2 Position = MakeFloat2(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index));
		const float Amplitude = LoadFloat(Buffers[2], Index);
		const float FeatureScale = LoadFloat(Buffers[3], Index);
		const int32 Seed = LoadInt32(Buffers[4], Index);
		const int32 Octaves = LoadInt32(Buffers[5], Index);
		const float Lacunarity = LoadFloat(Buffers[6], Index);
		const float Persistence = LoadFloat(Buffers[7], Index);
		float Value;

		Value = VoxelExtraNoiseNodes_SimplexFractal2D(Seed, Position / FeatureScale, Octaves, Lacunarity, Persistence) * Amplitude;

		StoreFloat(Buffers[8], Index, Value);
	}
}

export void VoxelNode_VoxelNode_TrueDistanceCellularNoise3D(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float3 Position = MakeFloat3(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index), LoadFloat(Buffers[2], Index));
		const float Amplitude = LoadFloat(Buffers[3], Index);
		const float FeatureScale = LoadFloat(Buffers[4], Index);
		const float Jitter = LoadFloat(Buffers[5], Index);
		const int32 Seed = LoadInt32(Buffers[6], Index);
		float Value;
		float3 CellPosition;

		Value = VoxelExtraNoiseNodes_GetTrueDistanceCellularNoise3D(Seed, Position / FeatureScale, Jitter, &CellPosition) * Amplitude;
		CellPosition = CellPosition * FeatureScale;

		StoreFloat(Buffers[7], Index, Value);
		StoreFloat(Buffers[8], Index, CellPosition.x);
		StoreFloat(Buffers[9], Index, CellPosition.y);
		StoreFloat(Buffers[10], Index, CellPosition.z);
	}
}