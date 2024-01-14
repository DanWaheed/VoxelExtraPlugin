#pragma once

#include "VoxelMinimal.h"

class FVoxelDependency;
class FVoxelHeightmapTextureData;
class UVoxelHeightmapTextureAsset;

class VOXELEXTRAGRAPHCORE_API FVoxelHeightmapTextureAssetData
{
public:
	const TSharedRef<FVoxelDependency> Dependency;
	const TWeakObjectPtr<const UVoxelHeightmapTextureAsset> WeakAsset;

	explicit FVoxelHeightmapTextureAssetData(const UVoxelHeightmapTextureAsset& Asset);

	TSharedPtr<const FVoxelHeightmapTextureData> GetTextureData() const;
	void SetTextureData(const TSharedPtr<const FVoxelHeightmapTextureData>& NewTextureData);

private:
	mutable FVoxelCriticalSection CriticalSection;
	TSharedPtr<const FVoxelHeightmapTextureData> TextureData_RequiresLock;
};