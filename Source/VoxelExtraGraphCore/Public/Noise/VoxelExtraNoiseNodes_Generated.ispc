// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeHelpers.isph"
#include "VoxelExtraNoiseNodesImpl.isph"

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

		Value = GetTrueDistanceCellularNoise3D(Seed, Position / FeatureScale, Jitter, &CellPosition) * Amplitude;
		CellPosition = CellPosition * FeatureScale;

		StoreFloat(Buffers[7], Index, Value);
		StoreFloat(Buffers[8], Index, CellPosition.x);
		StoreFloat(Buffers[9], Index, CellPosition.y);
		StoreFloat(Buffers[10], Index, CellPosition.z);
	}
}