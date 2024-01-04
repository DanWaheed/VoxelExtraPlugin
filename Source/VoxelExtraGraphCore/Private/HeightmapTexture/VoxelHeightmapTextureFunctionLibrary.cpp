#include "HeightmapTexture/VoxelHeightmapTextureFunctionLibrary.h"
#include "VoxelHeightmapTextureFunctionLibraryImpl.ispc.generated.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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

		if (const TSharedPtr<const FVoxelHeightmapTextureData> TextureData = FVoxelHeightmapTextureData::ReadTexture(*Object))
		{

			Struct.HeightmapTexture.Data = TextureData;
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

	return TextureData.Data->Min * ScaleZ;
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

	return TextureData.Data->Max * ScaleZ;
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

	return TextureData.Data->SizeX * (ScaleXY / 2)	;
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

	const TSharedPtr<const FVoxelHeightmapTextureData> HeightmapTextureData = TextureData.Data;
	if (!HeightmapTextureData)
	{
		VOXEL_MESSAGE(Error, "{0} texture failed to be read, this will occur if texture has mips or if the texture format is unsupported", this);
		return {};
	}

	const float CalculatedScaleZ = ScaleZ * (HeightmapTextureData->Max - HeightmapTextureData->Min) / MAX_uint16;
	const float OffsetZ = ScaleZ * HeightmapTextureData->Min;

	FVoxelFloatBufferStorage Result;
	Result.Allocate(Position.Num());

	ForeachVoxelBufferChunk_Parallel(Position.Num(), [&](const FVoxelBufferIterator& Iterator)
		{
			switch (Interpolation)
			{
			default: ensure(false);
			case EVoxelHeightmapInterpolationType::NearestNeighbor:
			{
				ispc::VoxelHeightmapFunctionLibrary_SampleHeightmap_NearestNeighbor(
					Position.X.GetData(Iterator),
					Position.X.IsConstant(),
					Position.Y.GetData(Iterator),
					Position.Y.IsConstant(),
					ScaleXY,
					ScaleZ,
					OffsetZ,
					HeightmapTextureData->SizeX,
					HeightmapTextureData->SizeY,
					HeightmapTextureData->Heights.GetData(),
					Iterator.Num(),
					Result.GetData(Iterator));
			}
			break;
			case EVoxelHeightmapInterpolationType::Bilinear:
			{
				ispc::VoxelHeightmapFunctionLibrary_SampleHeightmap_Bilinear(
					Position.X.GetData(Iterator),
					Position.X.IsConstant(),
					Position.Y.GetData(Iterator),
					Position.Y.IsConstant(),
					ScaleXY,
					ScaleZ,
					OffsetZ,
					HeightmapTextureData->SizeX,
					HeightmapTextureData->SizeY,
					HeightmapTextureData->Heights.GetData(),
					Iterator.Num(),
					Result.GetData(Iterator));
			}
			break;
			case EVoxelHeightmapInterpolationType::Bicubic:
			{
				ispc::VoxelHeightmapFunctionLibrary_SampleHeightmap_Bicubic(
					Position.X.GetData(Iterator),
					Position.X.IsConstant(),
					Position.Y.GetData(Iterator),
					Position.Y.IsConstant(),
					ScaleXY,
					ScaleZ,
					OffsetZ,
					BicubicSmoothness,
					HeightmapTextureData->SizeX,
					HeightmapTextureData->SizeY,
					HeightmapTextureData->Heights.GetData(),
					Iterator.Num(),
					Result.GetData(Iterator));
			}
			break;
			}
		});

	return FVoxelFloatBuffer::Make(Result);
}