// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "HeightmapTexture/VoxelHeightmapTextureAsset.h"
#include "HeightmapTexture/VoxelHeightmapTextureData.h"
#include "HeightmapTexture/VoxelHeightmapTextureAssetData.h"

#include "Misc/ConfigCacheIni.h"
#include "AssetRegistry/AssetData.h"
#include "Serialization/BufferArchive.h"

#if WITH_EDITOR
#include "AssetToolsModule.h"
#include "Editor/EditorEngine.h"
#include "EditorReimportHandler.h"
#include "DerivedDataCacheInterface.h"
#endif

DEFINE_VOXEL_FACTORY(UVoxelHeightmapTextureAsset);

#if WITH_EDITOR
VOXEL_RUN_ON_STARTUP_GAME(RegisterVoxelHeightmapTextureAssetOnReimport)
{
	FReimportManager::Instance()->OnPostReimport().AddLambda([](const UObject* Asset, bool bSuccess)
		{
			ForEachObjectOfClass<UVoxelHeightmapTextureAsset>([&](const UVoxelHeightmapTextureAsset& HeightmapTexture)
				{
					if (HeightmapTexture.Texture == Asset)
					{
						HeightmapTexture.GetData()->SetTextureData(HeightmapTexture.CreateTextureData());
					}
				});
		});
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<FVoxelHeightmapTextureAssetData> UVoxelHeightmapTextureAsset::GetData() const
{
	if (!PrivateData)
	{
		PrivateData = MakeVoxelShared<FVoxelHeightmapTextureAssetData>(*this);
	}
	if (!PrivateData->GetTextureData())
	{
		PrivateData->SetTextureData(CreateTextureData());
	}
	return PrivateData.ToSharedRef();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FVoxelHeightmapTextureData> UVoxelHeightmapTextureAsset::CreateTextureData() const
{
	VOXEL_FUNCTION_COUNTER();

	UTexture2D* LoadedTexture = Texture.LoadSynchronous();
	if (!LoadedTexture)
	{
		return nullptr;
	}

#if WITH_EDITOR
	FString KeySuffix;

	KeySuffix += "TX";
	KeySuffix += LoadedTexture->GetFullName();

	{
		FBufferArchive Writer;
		KeySuffix += "_" + FString::FromHexBlob(Writer.GetData(), Writer.Num());
	}

	const FString DerivedDataKey = FDerivedDataCacheInterface::BuildCacheKey(
		TEXT("VOXEL_HEIGHTMAP_TEXTURE"),
		TEXT("35EF96DA0A03463D8A5F7386E05BA4D5"),
		*KeySuffix);

	TArray<uint8> DerivedData;
	if (GetDerivedDataCacheRef().GetSynchronous(*DerivedDataKey, DerivedData, GetPathName()))
	{
		FMemoryReader Ar(DerivedData);

		const TSharedRef<FVoxelHeightmapTextureData> TextureData = MakeVoxelShared<FVoxelHeightmapTextureData>();
		TextureData->Serialize(Ar);
		return TextureData;
	}
	else
	{
		LOG_VOXEL(Log, "Heightmap Texture %s", *GetPathName());

		FScopedSlowTask SlowTask(1.f, FText::FromString("Heightmap Texture " + GetPathName()));
		// Only allow cancelling in PostEditChangeProperty
		SlowTask.MakeDialog(false, true);
		SlowTask.EnterProgressFrame();

		const TSharedPtr<FVoxelHeightmapTextureData> TextureData = FVoxelHeightmapTextureData::ReadTexture(*LoadedTexture);

		if (!TextureData)
		{
			return nullptr;
		}

		FBufferArchive Writer;
		ConstCast(*TextureData).Serialize(Writer);
		GetDerivedDataCacheRef().Put(*DerivedDataKey, Writer, GetPathName());

		return TextureData;
	}
#else
	return FVoxelHeightmapTextureData::ReadTexture(*LoadedTexture);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelHeightmapTextureAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	bool bCooked = Ar.IsCooking();
	Ar << bCooked;

	if (!bCooked ||
		IsTemplate() ||
		Ar.IsCountingMemory())
	{
		return;
	}

	if (Ar.IsLoading())
	{
		const TSharedRef<FVoxelHeightmapTextureData> TextureData = MakeVoxelShared<FVoxelHeightmapTextureData>();
		TextureData->Serialize(Ar);
		GetData()->SetTextureData(TextureData);
	}
#if WITH_EDITOR
	else if (Ar.IsSaving())
	{
		TSharedPtr<const FVoxelHeightmapTextureData> TextureData = GetData()->GetTextureData();
		if (!TextureData)
		{
			TextureData = CreateTextureData();
		}
		if (!TextureData)
		{
			TextureData = MakeVoxelShared<FVoxelHeightmapTextureData>();
		}
		ConstCast(*TextureData).Serialize(Ar);
	}
#endif
}

#if WITH_EDITOR
void UVoxelHeightmapTextureAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	VOXEL_FUNCTION_COUNTER();

	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
	{
		return;
	}

	// Recompute data
	// Do this next frame to be able to undo on cancel
	FVoxelUtilities::DelayedCall([=]
		{
			FScopedSlowTask SlowTask(1.f, FText::FromString("Heightmap Texture " + GetPathName()));
			SlowTask.MakeDialog(true, true);
			SlowTask.EnterProgressFrame();

			const TSharedPtr<FVoxelHeightmapTextureData> TextureData = CreateTextureData();

			if (SlowTask.ShouldCancel())
			{
				ensure(GEditor->UndoTransaction());
				return;
			}

			GetData()->SetTextureData(TextureData);
		});
}
#endif