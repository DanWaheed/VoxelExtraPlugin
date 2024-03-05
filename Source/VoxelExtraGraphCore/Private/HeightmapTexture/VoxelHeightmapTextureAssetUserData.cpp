#include "HeightmapTexture/VoxelHeightmapTextureAssetUserData.h"
#include "HeightmapTexture/VoxelHeightmapTextureAsset.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "VoxelMinimal/VoxelMessageManager.h"
#include "VoxelMinimal/Utilities/VoxelObjectUtilities.h"

UVoxelHeightmapTextureAsset* UVoxelHeightmapTextureAssetUserData::GetAsset(UTexture2D& Texture)
{
	if (const UVoxelHeightmapTextureAssetUserData* AssetUserData = Texture.GetAssetUserData<UVoxelHeightmapTextureAssetUserData>())
	{
		if (UVoxelHeightmapTextureAsset* Asset = AssetUserData->Asset.LoadSynchronous())
		{
			if (Asset->Texture == &Texture)
			{
				return Asset;
			}
		}
	}

	UVoxelHeightmapTextureAsset* Asset = nullptr;

	// Try to find an existing one
	{
		TArray<FAssetData> AssetDatas;
		FARFilter Filter;
		Filter.ClassPaths.Add(UVoxelHeightmapTextureAsset::StaticClass()->GetClassPathName());
		Filter.TagsAndValues.Add(GET_MEMBER_NAME_CHECKED(UVoxelHeightmapTextureAsset, Texture), Texture.GetPathName());

		const IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
		AssetRegistry.GetAssets(Filter, AssetDatas);

		if (AssetDatas.Num() > 1)
		{
			TArray<UObject*> Assets;
			for (const FAssetData& AssetData : AssetDatas)
			{
				Assets.Add(AssetData.GetAsset());
			}
			
			VOXEL_MESSAGE(Warning, "More than 1 texture asset for mesh {0} found: {1}", Texture, Assets);
		}

		for (const FAssetData& AssetData : AssetDatas)
		{
			UObject* Object = AssetData.GetAsset();
			if (!ensure(Object) ||
				!ensure(Object->IsA<UVoxelHeightmapTextureAsset>()))
			{
				continue;
			}

			Asset = CastChecked<UVoxelHeightmapTextureAsset>(Object);
			break;
		}

		if (!Asset &&
			AssetRegistry.IsLoadingAssets())
		{
			// Otherwise new assets are created for the same mesh
			VOXEL_MESSAGE(Error, "Asset registry is still loading assets - please refresh the voxel actor (Ctrl F5) once assets are loaded");
			return nullptr;
		}
	}

	if (!Asset)
	{
#if WITH_EDITOR
		// Create a new heightmap asset

		FString PackageName = FPackageName::ObjectPathToPackageName(Texture.GetPathName());
		if (!PackageName.StartsWith("/Game/"))
		{
			// Don't create assets in the engine
			PackageName = "/Game/HeightmapTextures/" + Texture.GetName();
		}

		Asset = FVoxelUtilities::CreateNewAsset_Direct<UVoxelHeightmapTextureAsset>(PackageName, "_HeightmapTexture");

		if (!Asset)
		{
			return nullptr;
		}

		Asset->Texture = &Texture;
#else
		// TODO Support this
		VOXEL_MESSAGE(Error, "Trying to create a texture from {0} at runtime. Please do it in editor instead", Texture);
		return nullptr;
#endif
	}

	ensure(Asset->Texture == &Texture);

	UVoxelHeightmapTextureAssetUserData* AssetUserData = NewObject<UVoxelHeightmapTextureAssetUserData>(&Texture);
	AssetUserData->Asset = Asset;

	Texture.AddAssetUserData(AssetUserData);

	return Asset;
}
