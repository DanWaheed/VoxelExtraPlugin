#include "HeightmapTexture/VoxelHeightmapTextureData.h"

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelHeightmapTextureData);

int64 FVoxelHeightmapTextureData::GetAllocatedSize() const
{
	int64 AllocatedSize = sizeof(*this);
	AllocatedSize += Heights.GetAllocatedSize();
	return AllocatedSize;
}

void FVoxelHeightmapTextureData::Serialize(FArchive& Ar)
{
	VOXEL_FUNCTION_COUNTER();

	using FVersion = DECLARE_VOXEL_VERSION
	(
		FirstVersion,
		RemoveMaxSmoothness,
		AddRanges,
		AddRangeMips,
		RemoveRangeMips,
		RemoveSerializedRangeChunkSize,
		AddMaxSmoothness,
		AddNormals,
		SwitchToFVoxelBox
	);

	int32 Version = FVersion::LatestVersion;
	Ar << Version;

	if (Version < FVersion::AddNormals)
	{
		return;
	}

	Ar << SizeX;
	Ar << SizeY;
	Ar << Min;
	Ar << Max;

	Heights.BulkSerialize(Ar);

	UpdateStats();
}

TSharedPtr<const FVoxelHeightmapTextureData> FVoxelHeightmapTextureData::ReadTexture(
	const UTexture2D& Texture)
{
	VOXEL_FUNCTION_COUNTER();
#if WITH_EDITOR
	const TextureMipGenSettings InitialMipGenSettings = Texture.MipGenSettings;
	if (InitialMipGenSettings != TextureMipGenSettings::TMGS_NoMipmaps)
	{
		return nullptr;
	}
#endif

	const FTexturePlatformData* PlatformData = Texture.GetPlatformData();
	const FByteBulkData TextureData = PlatformData->Mips[0].BulkData;

	TArray<uint16> ProcessedHeights{};

	if (const uint16_t* IntegerImageData = reinterpret_cast<const uint16_t*>(TextureData.LockReadOnly()))
	{
		const int32 PixelCount = PlatformData->SizeX * PlatformData->SizeY;
		ProcessedHeights.SetNum(PixelCount);

		if (PlatformData->PixelFormat == PF_B8G8R8A8)
		{
			const FColor* ColorTextureData = reinterpret_cast<const FColor*>(IntegerImageData);
			TArray<FColor> ColorData{};
			ColorData.SetNum(PixelCount);

			for (int32 D = 0; D < PixelCount; ++D)
			{
				FColor PixelColor = ColorTextureData[D];
				ColorData[D] = PixelColor;
			}

			for (int32 Index = 0; Index != ColorData.Num(); ++Index)
			{
				const FColor color = ColorData[Index];

				// Use R Value from Texture
				ProcessedHeights[Index] = color.R;
			}
		}
		else if (PlatformData->PixelFormat == PF_G16)
		{
			const uint16* G16Data = reinterpret_cast<const uint16*>(IntegerImageData);

			for (int32 D = 0; D < PixelCount; ++D)
			{
				ProcessedHeights[D] = G16Data[D];
			}
		}
		else if (PlatformData->PixelFormat == PF_G8)
		{
			const uint8* G8Data = reinterpret_cast<const uint8*>(IntegerImageData);

			for (int32 D = 0; D < PixelCount; ++D)
			{
				ProcessedHeights[D] = G8Data[D];
			}
		}
		else if (PlatformData->PixelFormat == PF_FloatRGBA)
		{
			const FFloat16Color* FloatColorData = reinterpret_cast<const FFloat16Color*>(IntegerImageData);
			TArray<FFloat16Color> ColorData{};
			ColorData.SetNum(PixelCount);

			for (int32 D = 0; D < PixelCount; ++D)
			{
				FFloat16Color PixelColor = FloatColorData[D];
				ColorData[D] = PixelColor;
			}

			for (int32 Index = 0; Index != ColorData.Num(); ++Index)
			{
				const FFloat16Color color = ColorData[Index];

				// Use R Value from Texture
				ProcessedHeights[Index] = color.R;
			}
		}
	}

	if (PlatformData->PixelFormat == PF_G8 || PlatformData->PixelFormat == PF_B8G8R8A8)
	{
		ProcessedHeights = FVoxelHeightmapTextureData::ScaleData(ProcessedHeights, 128);
		ProcessedHeights = FVoxelHeightmapTextureData::BoxBlur(ProcessedHeights, 50);
	}

	TextureData.Unlock();

	// LINQ .All(x => x == 0) Equivalent
	bool HasNoData = true;
	for (int32 Index = 0; Index != ProcessedHeights.Num(); ++Index)
	{
		uint16 Value = ProcessedHeights[Index];
		if (Value != 0)
		{
			HasNoData = false;
			break;
		}
	}

	if (ProcessedHeights.Num() == 0 || HasNoData)
	{
		return nullptr;
	}

	uint16 Min = ProcessedHeights[0];
	uint16 Max = ProcessedHeights[0];
	for (const float Height : ProcessedHeights)
	{
		Min = FMath::Min(Min, Height);
		Max = FMath::Max(Max, Height);
	}

	const TSharedRef<FVoxelHeightmapTextureData> HeightmapTextureData = MakeVoxelShared<FVoxelHeightmapTextureData>();
	HeightmapTextureData->SizeX = PlatformData->SizeX;
	HeightmapTextureData->SizeY = PlatformData->SizeY;
	HeightmapTextureData->Min = Min;
	HeightmapTextureData->Max = Max;
	HeightmapTextureData->Heights = TVoxelArray<uint16>(ProcessedHeights);

	return HeightmapTextureData;
}