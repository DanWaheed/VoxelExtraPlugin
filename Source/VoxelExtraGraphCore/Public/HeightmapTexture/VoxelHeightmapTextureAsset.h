#pragma once

#include "VoxelHeightmapTextureAsset.generated.h"

class FVoxelHeightmapTextureData;
class UVoxelHeightmapTextureAsset;
class FVoxelHeightmapTextureAssetData;

UCLASS(BlueprintType, meta = (VoxelAssetType, AssetColor = Red))
class VOXELEXTRAGRAPHCORE_API UVoxelHeightmapTextureAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Voxel", AssetRegistrySearchable)
	TSoftObjectPtr<UTexture2D> Texture;

	TSharedRef<FVoxelHeightmapTextureAssetData> GetData() const;
	TSharedPtr<FVoxelHeightmapTextureData> CreateTextureData() const;

public:
	//~ Begin UObject Interface
	virtual void Serialize(FArchive& Ar) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

private:
	mutable TSharedPtr<FVoxelHeightmapTextureAssetData> PrivateData;
};