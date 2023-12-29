#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelObjectPinType.h"
#include "VoxelFunctionLibrary.h"
#include "VoxelHeightmapTextureData.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "Heightmap/VoxelHeightmapFunctionLibrary.h"
#include "VoxelHeightmapTextureFunctionLibrary.generated.h"

USTRUCT()
struct VOXELEXTRAGRAPHCORE_API FVoxelHeightmapResult
{
	GENERATED_BODY()

	FVoxelFloatBuffer Heights;
	uint16 Min;
	uint16 Max;
	uint16 Width;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelHeightmapResult);

USTRUCT()
struct VOXELEXTRAGRAPHCORE_API FVoxelHeightmapTexture
{
	GENERATED_BODY()

	TSharedPtr<const FVoxelHeightmapTextureData> Data;
};

USTRUCT()
struct VOXELEXTRAGRAPHCORE_API FVoxelHeightmapTextureObject
{
	GENERATED_BODY()

	TWeakObjectPtr<UTexture2D> Texture;
	FVoxelHeightmapTexture HeightmapTexture;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelHeightmapTexture);

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelHeightmapTextureObject);

USTRUCT()
struct VOXELEXTRAGRAPHCORE_API FVoxelHeightmapTextureObjectPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelHeightmapTextureObject, UTexture2D);
};

UCLASS()
class VOXELEXTRAGRAPHCORE_API UVoxelHeightmapTextureFunctionLibrary : public UVoxelFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(Category = "Heightmap Texture")
	FVoxelHeightmapTexture MakeHeightmapFromTexture(const FVoxelHeightmapTextureObject& Texture) const;

	UFUNCTION(Category = "Heightmap Texture")
	int32 GetMinHeightFromHeightmapTexture(const FVoxelHeightmapTexture& TextureData) const;

	UFUNCTION(Category = "Heightmap Texture")
	int32 GetMaxHeightFromHeightmapTexture(const FVoxelHeightmapTexture& TextureData) const;

	UFUNCTION(Category = "Heightmap Texture")
	int32 GetWidthFromHeightmapTexture(const FVoxelHeightmapTexture& TextureData) const;

	// Will clamp position if outside of the heightmap bounds
	// Heightmap is centered, ie position is between -Size/2 and Size/2
	// @param    BicubicSmoothness    Smoothness of the bicubic interpolation, between 0 and 1
	UFUNCTION(Category = "Heightmap Texture", meta = (AdvancedDisplay = "Interpolation, BicubicSmoothness"))
	FVoxelFloatBuffer SampleHeightmapFromTexture(
		const FVoxelVector2DBuffer& Position,
		const FVoxelHeightmapTexture& TextureData,
		const float ScaleZ = 2.f,
		const float ScaleXY = 100.f,
		EVoxelHeightmapInterpolationType Interpolation = EVoxelHeightmapInterpolationType::Bicubic,
		float BicubicSmoothness = 1.f) const;
};