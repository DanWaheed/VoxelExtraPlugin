#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelObjectPinType.h"
#include "VoxelFunctionLibrary.h"
#include "VoxelHeightmapTextureData.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "FunctionLibrary/VoxelHeightmapFunctionLibrary.h"
#include "VoxelHeightmapTextureAssetUserData.h"
#include "VoxelHeightmapTextureAssetData.h"
#include "VoxelHeightmapTextureFunctionLibrary.generated.h"

USTRUCT()
struct VOXELEXTRAGRAPHCORE_API FVoxelHeightmapResult
{
	GENERATED_BODY()

	FVoxelFloatBuffer Heights;
	FVoxelLinearColorBuffer Colors;
	uint16 Min;
	uint16 Max;
	uint16 Width;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelHeightmapResult);

USTRUCT()
struct VOXELEXTRAGRAPHCORE_API FVoxelHeightmapTexture
{
	GENERATED_BODY()

	TWeakObjectPtr<UVoxelHeightmapTextureAsset> Asset;
	TSharedPtr<const FVoxelHeightmapTextureAssetData> Data;
};

USTRUCT()
struct VOXELEXTRAGRAPHCORE_API FVoxelHeightmapTextureObject
{
	GENERATED_BODY()

	TWeakObjectPtr<UTexture2D> Texture;
	FVoxelHeightmapTexture HeightmapTexture;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelHeightmapTexture);

USTRUCT()
struct VOXELEXTRAGRAPHCORE_API FVoxelHeightmapTexturePinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelHeightmapTexture, UVoxelHeightmapTextureAsset);
};

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
	float GetMinHeightFromHeightmapTexture(
		const FVoxelHeightmapTexture& TextureData,
		const float ScaleZ = 2.f) const;

	UFUNCTION(Category = "Heightmap Texture")
	float GetMaxHeightFromHeightmapTexture(
		const FVoxelHeightmapTexture& TextureData,
		const float ScaleZ = 2.f) const;

	UFUNCTION(Category = "Heightmap Texture")
	float GetWidthFromHeightmapTexture(
		const FVoxelHeightmapTexture& TextureData,
		const float ScaleXY = 100.f) const;

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

	// Will clamp position if outside of the heightmap bounds
	// Heightmap is centered, ie position is between -Size/2 and Size/2
	UFUNCTION(Category = "Heightmap Texture", meta = (AdvancedDisplay = "Interpolation, BicubicSmoothness"))
	FVoxelLinearColorBuffer SampleWeightmapFromTexture(
		const FVoxelVector2DBuffer& Position,
		const FVoxelHeightmapTexture& TextureData,
		const float ScaleZ = 2.f,
		const float ScaleXY = 100.f) const;
};