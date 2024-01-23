// Copyright Voxel Plugin SAS. All Rights Reserved.

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