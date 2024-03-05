#pragma once

#include "Engine/AssetUserData.h"
#include "VoxelHeightmapTextureAssetUserData.generated.h"

class UVoxelHeightmapTextureAsset;

UCLASS()
class VOXELEXTRAGRAPHCORE_API UVoxelHeightmapTextureAssetUserData : public UAssetUserData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Voxel")
	TSoftObjectPtr<UVoxelHeightmapTextureAsset> Asset;

	static UVoxelHeightmapTextureAsset* GetAsset(UTexture2D& Texture);
};