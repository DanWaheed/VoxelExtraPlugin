#include "HeightmapTexture/VoxelHeightmapTextureFunctionLibrary.h"
#include "Utilities/VoxelBufferGatherer.h"
#include "VoxelBufferBuilder.h"
#include "EdGraph/EdGraphNode.h"
#include "VoxelDependencyTracker.h"
#include "VoxelHeightmapTextureFunctionLibraryImpl.ispc.generated.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelHeightmapTexturePinType::Convert(
	const bool bSetObject,
	TWeakObjectPtr<UVoxelHeightmapTextureAsset>& Object,
	FVoxelHeightmapTexture& Struct) const
{
	if (bSetObject)
	{
		Object = Struct.Asset;
	}
	else
	{
		Struct.Asset = Object;
		Struct.Data = Object->GetData();
	}
}

void FVoxelHeightmapTextureObjectPinType::Convert(
	const bool bSetObject,
	TWeakObjectPtr<UTexture2D>& Object,
	FVoxelHeightmapTextureObject& Struct) const
{
	if (bSetObject)
	{
		Object = Struct.Texture;
	}
	else
	{
		Struct.Texture = Object;

		if (UVoxelHeightmapTextureAsset* Asset = UVoxelHeightmapTextureAssetUserData::GetAsset(*Object))
		{
			Struct.HeightmapTexture.Asset = Asset;
			Struct.HeightmapTexture.Data = Asset->GetData();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelHeightmapTexture UVoxelHeightmapTextureFunctionLibrary::MakeHeightmapFromTexture(const FVoxelHeightmapTextureObject& Texture) const
{
	if (Texture.Texture.IsExplicitlyNull())
	{
		VOXEL_MESSAGE(Error, "{0}: texture is null", this);
		return {};
	}

	return Texture.HeightmapTexture;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

float UVoxelHeightmapTextureFunctionLibrary::GetMinHeightFromHeightmapTexture(
	const FVoxelHeightmapTexture& TextureData,
	float ScaleZ) const
{
	if (TextureData.Data == nullptr)
	{
		VOXEL_MESSAGE(Error, "{0}: texture is null", this);
		return {};
	}

	GetQuery().GetDependencyTracker().AddDependency(TextureData.Data->Dependency);
	const TSharedPtr<const FVoxelHeightmapTextureData> Data = TextureData.Data->GetTextureData();
	if (!Data)
	{
		VOXEL_MESSAGE(Error, "{0}: {1}: texture failed to be read, this will occur if texture has mips or if the texture format is unsupported", this, TextureData.Asset);
		return {};
	}

	return Data->Min * ScaleZ;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

float UVoxelHeightmapTextureFunctionLibrary::GetMaxHeightFromHeightmapTexture(
	const FVoxelHeightmapTexture& TextureData,
	float ScaleZ) const
{
	if (TextureData.Data == nullptr)
	{
		VOXEL_MESSAGE(Error, "{0}: texture is null", this);
		return {};
	}

	GetQuery().GetDependencyTracker().AddDependency(TextureData.Data->Dependency);
	const TSharedPtr<const FVoxelHeightmapTextureData> Data = TextureData.Data->GetTextureData();
	if (!Data)
	{
		VOXEL_MESSAGE(Error, "{0}: {1}: texture failed to be read, this will occur if texture has mips or if the texture format is unsupported", this, TextureData.Asset);
		return {};
	}

	return Data->Max * ScaleZ;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

float UVoxelHeightmapTextureFunctionLibrary::GetWidthFromHeightmapTexture(
	const FVoxelHeightmapTexture& TextureData,
	float ScaleXY) const
{
	if (TextureData.Data == nullptr)
	{
		VOXEL_MESSAGE(Error, "{0}: texture is null", this);
		return {};
	}

	GetQuery().GetDependencyTracker().AddDependency(TextureData.Data->Dependency);
	const TSharedPtr<const FVoxelHeightmapTextureData> Data = TextureData.Data->GetTextureData();
	if (!Data)
	{
		VOXEL_MESSAGE(Error, "{0}: {1}: texture failed to be read, this will occur if texture has mips or if the texture format is unsupported", this, TextureData.Asset);
		return {};
	}

	return Data->SizeX * (ScaleXY / 2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFloatBuffer UVoxelHeightmapTextureFunctionLibrary::SampleHeightmapFromTexture(
	const FVoxelVector2DBuffer& Position,
	const FVoxelHeightmapTexture& TextureData,
	const float ScaleZ,
	const float ScaleXY,
	const EVoxelHeightmapInterpolationType Interpolation,
	const float BicubicSmoothness) const
{
	VOXEL_SCOPE_COUNTER_FORMAT("SampleHeightmap %s Num=%d", *UEnum::GetValueAsString(Interpolation), Position.Num());

	if (TextureData.Data == nullptr)
	{
		VOXEL_MESSAGE(Error, "{0}: texture is null", this);
		return {};
	}

	GetQuery().GetDependencyTracker().AddDependency(TextureData.Data->Dependency);
	const TSharedPtr<const FVoxelHeightmapTextureData> Data = TextureData.Data->GetTextureData();
	if (!Data)
	{
		VOXEL_MESSAGE(Error, "{0}: {1}: texture failed to be read, this will occur if texture has mips or if the texture format is unsupported", this, TextureData.Asset);
		return {};
	}

	const float CalculatedScaleZ = ScaleZ * (Data->Max - Data->Min) / MAX_uint16;
	const float OffsetZ = ScaleZ * Data->Min;

	FVoxelFloatBufferStorage Result;
	Result.Allocate(Position.Num());

	ForeachVoxelBufferChunk_Parallel(Position.Num(), [&](const FVoxelBufferIterator& Iterator)
		{
			switch (Interpolation)
			{
			default: ensure(false);
			case EVoxelHeightmapInterpolationType::NearestNeighbor:
			{
				ispc::VoxelExtraHeightmapFunctionLibrary_SampleHeightmap_NearestNeighbor(
					Position.X.GetData(Iterator),
					Position.X.IsConstant(),
					Position.Y.GetData(Iterator),
					Position.Y.IsConstant(),
					ScaleXY,
					ScaleZ,
					OffsetZ,
					Data->SizeX,
					Data->SizeY,
					Data->Heights.GetData(),
					Iterator.Num(),
					Result.GetData(Iterator));
			}
			break;
			case EVoxelHeightmapInterpolationType::Bilinear:
			{
				ispc::VoxelExtraHeightmapFunctionLibrary_SampleHeightmap_Bilinear(
					Position.X.GetData(Iterator),
					Position.X.IsConstant(),
					Position.Y.GetData(Iterator),
					Position.Y.IsConstant(),
					ScaleXY,
					ScaleZ,
					OffsetZ,
					Data->SizeX,
					Data->SizeY,
					Data->Heights.GetData(),
					Iterator.Num(),
					Result.GetData(Iterator));
			}
			break;
			case EVoxelHeightmapInterpolationType::Bicubic:
			{
				ispc::VoxelExtraHeightmapFunctionLibrary_SampleHeightmap_Bicubic(
					Position.X.GetData(Iterator),
					Position.X.IsConstant(),
					Position.Y.GetData(Iterator),
					Position.Y.IsConstant(),
					ScaleXY,
					ScaleZ,
					OffsetZ,
					BicubicSmoothness,
					Data->SizeX,
					Data->SizeY,
					Data->Heights.GetData(),
					Iterator.Num(),
					Result.GetData(Iterator));
			}
			break;
			}
		});

	return FVoxelFloatBuffer::Make(Result);
}

FVoxelLinearColorBuffer UVoxelHeightmapTextureFunctionLibrary::SampleWeightmapFromTexture(
	const FVoxelVector2DBuffer& Position,
	const FVoxelHeightmapTexture& TextureData,
	const float ScaleZ,
	const float ScaleXY) const
{
	if (TextureData.Data == nullptr)
	{
		VOXEL_MESSAGE(Error, "{0}: texture is null", this);
		return {};
	}

	GetQuery().GetDependencyTracker().AddDependency(TextureData.Data->Dependency);
	const TSharedPtr<const FVoxelHeightmapTextureData> Data = TextureData.Data->GetTextureData();
	if (!Data)
	{
		VOXEL_MESSAGE(Error, "{0}: {1}: texture failed to be read, this will occur if texture has mips or if the texture format is unsupported", this, TextureData.Asset);
		return {};
	}

	const float CalculatedScaleZ = ScaleZ * (Data->Max - Data->Min) / MAX_uint16;
	const float OffsetZ = ScaleZ * Data->Min;

	FVoxelInt32BufferStorage Indices;
	Indices.Allocate(Position.Num());

	ForeachVoxelBufferChunk_Parallel(Position.Num(), [&](const FVoxelBufferIterator& Iterator)
		{
			ispc::VoxelExtraHeightmapFunctionLibrary_SampleHeightmap_NearestNeighbor_GetIndices(
				Position.X.GetData(Iterator),
				Position.X.IsConstant(),
				Position.Y.GetData(Iterator),
				Position.Y.IsConstant(),
				ScaleXY,
				ScaleZ,
				OffsetZ,
				Data->SizeX,
				Data->SizeY,
				Iterator.Num(),
				Indices.GetData(Iterator));
		});

	FVoxelFloatBufferStorage InRs;
	FVoxelFloatBufferStorage InGs;
	FVoxelFloatBufferStorage InBs;
	FVoxelFloatBufferStorage InAs;
	InRs.Allocate(Position.Num());
	InGs.Allocate(Position.Num());
	InBs.Allocate(Position.Num());
	InAs.Allocate(Position.Num());
	for (int32 ValueIndex = 0; ValueIndex < Position.Num(); ValueIndex++)
	{
		FLinearColor CurrentColor = Data->Colors[Indices[ValueIndex]];
		InRs[ValueIndex] = CurrentColor.R;
		InGs[ValueIndex] = CurrentColor.G;
		InBs[ValueIndex] = CurrentColor.B;
		InAs[ValueIndex] = CurrentColor.A;
	}

	return FVoxelLinearColorBuffer::Make(InRs, InGs, InBs, InAs);
}