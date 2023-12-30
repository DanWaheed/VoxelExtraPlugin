#pragma once

#include "VoxelMinimal.h"
#include "VoxelBlur.ispc.generated.h"

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

private:
	static TArray<uint16> BoxBlur(const TArray<uint16>& InputArray, int32 Amount)
	{
		int32 ArraySize = InputArray.Num();
		TArray<uint16> SmoothedArray;
		SmoothedArray.SetNumUninitialized(ArraySize);

		ispc::VoxelBoxBlur(ArraySize, InputArray.GetData(), Amount, SmoothedArray.GetData());

		return SmoothedArray;
	}

    static TArray<uint16> ImproveBitDepth(const TArray<uint16>& InputArray)
    {
        TArray<uint16> Output;
        Output.SetNum(InputArray.Num());

        for (int32 i = 0; i < InputArray.Num(); ++i)
        {
            uint16 PixelValue = InputArray[i];
            Output[i] = PixelValue * 128;
        }

        return Output;
    }
};