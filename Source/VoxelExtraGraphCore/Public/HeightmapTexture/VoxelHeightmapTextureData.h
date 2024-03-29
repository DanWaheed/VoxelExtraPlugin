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
	TVoxelArray<float> Heights;
	TVoxelArray<FFloat16Color> Colors;

	FVoxelHeightmapTextureData() = default;

	VOXEL_ALLOCATED_SIZE_TRACKER(STAT_VoxelHeightmapTextureData);

	int64 GetAllocatedSize() const;
	void Serialize(FArchive& Ar);

	static TSharedPtr<FVoxelHeightmapTextureData> ReadTexture(const UTexture2D& Texture);

private:
	static TArray<float> BoxBlur(const TVoxelArray<float>& InputArray, float Amount)
	{
		int32 ArraySize = InputArray.Num();
		TArray<float> SmoothedArray;
		SmoothedArray.SetNumUninitialized(ArraySize);

		ispc::VoxelBoxBlur(ArraySize, InputArray.GetData(), Amount, SmoothedArray.GetData());

		return SmoothedArray;
	}

    static TArray<float> ScaleData(const TVoxelArray<float>& InputArray, float Scale)
    {
        TArray<float> Output;
        Output.SetNum(InputArray.Num());

        for (int32 i = 0; i < InputArray.Num(); ++i)
        {
	        const float PixelValue = InputArray[i];
            Output[i] = PixelValue * Scale;
        }

        return Output;
    }
};