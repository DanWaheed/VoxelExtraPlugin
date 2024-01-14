#include "HeightmapTexture/VoxelHeightmapTextureAssetData.h"
#include "HeightmapTexture/VoxelHeightmapTextureAsset.h"
#include "VoxelDependency.h"

FVoxelHeightmapTextureAssetData::FVoxelHeightmapTextureAssetData(const UVoxelHeightmapTextureAsset& Asset)
	: Dependency(FVoxelDependency::Create(STATIC_FNAME("HeightmapTexture"), Asset.GetFName()))
	, WeakAsset(&Asset)
{
}

TSharedPtr<const FVoxelHeightmapTextureData> FVoxelHeightmapTextureAssetData::GetTextureData() const
{
	VOXEL_SCOPE_LOCK(CriticalSection);
	return TextureData_RequiresLock;
}

void FVoxelHeightmapTextureAssetData::SetTextureData(const TSharedPtr<const FVoxelHeightmapTextureData>& NewTextureData)
{
	{
		VOXEL_SCOPE_LOCK(CriticalSection);
		TextureData_RequiresLock = NewTextureData;
	}

	Dependency->Invalidate();
}