#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelObjectPinType.h"
#include "VoxelFunctionLibrary.h"
#include "VoxelHeightmapTextureData.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelHeightmapFunctionLibrary.h"
#include "VoxelHeightmapTextureFunctionLibrary.generated.h"

UENUM()
enum class EVoxelHeightmapInterpolationType : uint8
{
	// Fastest, rounds sample position
	NearestNeighbor,
	// Bilinear interpolation
	// Smoother than nearest, but will have a visible grid pattern
	// 2.5x slower than nearest
	Bilinear,
	// Bicubic interpolation
	// Best quality, smooth results with no artifacts
	// 4x slower than nearest, 1.6x slower than bilinear
	Bicubic
};

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
};