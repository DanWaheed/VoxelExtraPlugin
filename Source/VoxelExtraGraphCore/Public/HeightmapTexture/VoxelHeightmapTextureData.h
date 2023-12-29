#pragma once

#include "VoxelMinimal.h"

DECLARE_VOXEL_MEMORY_STAT(VOXELEXTRAGRAPHCORE_API, STAT_VoxelHeightmapTextureData, "Voxel Heightmap Texture Data Memory");

class VOXELEXTRAGRAPHCORE_API FVoxelHeightmapTextureData
{
public:
	uint32 SizeX;
	uint32 SizeY;
	uint16 Min;
	uint16 Max;
	TVoxelArray<uint16> Heights;

	FVoxelHeightmapTextureData() = default;

	VOXEL_ALLOCATED_SIZE_TRACKER(STAT_VoxelHeightmapTextureData);

	int64 GetAllocatedSize() const;
	void Serialize(FArchive& Ar);

	static TSharedPtr<const FVoxelHeightmapTextureData> ReadTexture(
		const UTexture2D& Texture);
};