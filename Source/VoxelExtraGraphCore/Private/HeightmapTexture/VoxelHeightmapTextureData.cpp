#include "HeightmapTexture/VoxelHeightmapTextureData.h"

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelHeightmapTextureData);

FORCEINLINE FArchive& operator<<(FArchive& Ar, FFloat16Color& Color)
{
	Ar << Color.R;
	Ar << Color.G;
	Ar << Color.B;
	Ar << Color.A;
	return Ar;
}

int64 FVoxelHeightmapTextureData::GetAllocatedSize() const
{
	int64 AllocatedSize = sizeof(*this);
	AllocatedSize += Heights.GetAllocatedSize();
	AllocatedSize += Colors.GetAllocatedSize();
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
	Colors.BulkSerialize(Ar);

	UpdateStats();
}
TSharedPtr<FVoxelHeightmapTextureData> FVoxelHeightmapTextureData::ReadTexture(const UTexture2D& Texture)
{
	VOXEL_FUNCTION_COUNTER();
#if WITH_EDITOR
	if (const TextureMipGenSettings InitialMipGenSettings = Texture.MipGenSettings; InitialMipGenSettings != TMGS_NoMipmaps)
	{
		return nullptr;
	}
#endif

	const FTexturePlatformData* PlatformData = Texture.GetPlatformData();
	const FByteBulkData TextureData = PlatformData->Mips[0].BulkData;

	TVoxelArray<FFloat16Color> Colors{};
	TVoxelArray<float> ProcessedHeights{};

	if (const uint16_t* IntegerImageData = static_cast<const uint16_t*>(TextureData.LockReadOnly()))
	{
		const int32 PixelCount = PlatformData->SizeX * PlatformData->SizeY;
		ProcessedHeights.SetNumUninitialized(PixelCount);
		Colors.SetNumUninitialized(PixelCount);

		if (PlatformData->PixelFormat == PF_B8G8R8A8)
		{
			const FColor* ColorTextureData = reinterpret_cast<const FColor*>(IntegerImageData);

			for (int32 D = 0; D < PixelCount; ++D)
			{
				const FFloat16Color Color = FFloat16Color(ColorTextureData[D]);
				Colors[D] = Color;
			}
		}
		else if (PlatformData->PixelFormat == PF_G16)
		{
			const uint16* G16Data = IntegerImageData;

			for (int32 D = 0; D < PixelCount; ++D)
			{
				const uint16 Data = G16Data[D];
				FFloat16Color Color;
				Color.A = Data / 65535;
				Colors[D] = Color;
				ProcessedHeights[D] = Data;
			}
		}
		else if (PlatformData->PixelFormat == PF_G8)
		{
			const uint8* G8Data = reinterpret_cast<const uint8*>(IntegerImageData);

			for (int32 D = 0; D < PixelCount; ++D)
			{
				const uint8 Data = G8Data[D];
				FFloat16Color Color;
				Color.A = Data / 255;
				Colors[D] = Color;
				ProcessedHeights[D] = Data;
			}
		}
		else if (PlatformData->PixelFormat == PF_FloatRGBA)
		{
			const FFloat16Color* FloatColorData = reinterpret_cast<const FFloat16Color*>(IntegerImageData);

			for (int32 D = 0; D < PixelCount; ++D)
			{
				const FFloat16Color Color = FloatColorData[D];
				Colors[D] = Color;
				ProcessedHeights[D] = Color.A * 65535;
			}
		}
		else if (PlatformData->PixelFormat == PF_BC6H)
		{
			const FColor* ColorData = reinterpret_cast<const FColor*>(IntegerImageData);

			for (int32 D = 0; D < PixelCount; ++D)
			{
				const FFloat16Color Color = FFloat16Color(ColorData[D]);
				Colors[D] = Color;
				ProcessedHeights[D] = Color.A * 65535;
			}
		}
	}

	if (PlatformData->PixelFormat == PF_G8 || PlatformData->PixelFormat == PF_B8G8R8A8)
	{
		ProcessedHeights = ScaleData(ProcessedHeights, 128);
		ProcessedHeights = BoxBlur(ProcessedHeights, 50);
	}

	TextureData.Unlock();

	// LINQ .All(x => x == 0) Equivalent
	bool HasNoData = true;
	for (int32 Index = 0; Index != ProcessedHeights.Num(); ++Index)
	{
		if (int32 Value = ProcessedHeights[Index]; Value != 0)
		{
			HasNoData = false;
			break;
		}
	}

	if (ProcessedHeights.Num() == 0 || HasNoData)
	{
		return nullptr;
	}

	float Min = ProcessedHeights[0];
	float Max = ProcessedHeights[0];
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
	HeightmapTextureData->Heights = ProcessedHeights;
	HeightmapTextureData->Colors = Colors;

	return HeightmapTextureData;
}