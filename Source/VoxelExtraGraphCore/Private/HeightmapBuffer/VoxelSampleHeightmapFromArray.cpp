#include "HeightmapBuffer/VoxelSampleHeightmapFromArray.h"
#include "Math/UnrealMathUtility.h"
#include "Utilities/VoxelBufferGatherer.h"
#include "VoxelBufferBuilder.h"
#include "EdGraph/EdGraphNode.h"
#include "VoxelDependencyTracker.h"
#include "VoxelHeightmapTextureFunctionLibraryImpl.ispc.generated.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_SampleHeightmapFromArray, Result)
{
	const TValue<FVoxelVector2DBuffer> Position = GetNodeRuntime().Get<FVoxelVector2DBuffer>(PositionPin, Query);
	const TValue<FVoxelFloatBuffer> Heights = GetNodeRuntime().Get<FVoxelFloatBuffer>(HeightsPin, Query);
	const TValue<float> Offset = GetNodeRuntime().Get<float>(OffsetPin, Query);
	const TValue<float> ScaleZ = GetNodeRuntime().Get<float>(ScaleZPin, Query);
	const TValue<float> ScaleXY = GetNodeRuntime().Get<float>(ScaleXYPin, Query);
	const TValue<EVoxelHeightmapInterpolationType> Interpolation = GetNodeRuntime().Get<EVoxelHeightmapInterpolationType>(InterpolationPin, Query);
	const TValue<float> BicubicSmoothness = GetNodeRuntime().Get<float>(BicubicSmoothnessPin, Query);

	return VOXEL_ON_COMPLETE(Position, Heights, Offset, ScaleZ, ScaleXY, Interpolation, BicubicSmoothness)
	{
		int SquareSize = FMath::RoundToInt(FMath::Sqrt(static_cast<float>(Heights.Num())));
		if (FMath::Square(SquareSize) != Heights.Num())
		{
			VOXEL_MESSAGE(Error, "{0}: The array is not a perfect square, count was {1}", this, Heights.Num());
			return {};
		}

		FVoxelFloatBufferStorage Result;
		Result.Allocate(Position.Num());

		TArray<float> HeightsArray;
		for (int32 Index = 0; Index < Heights.Num(); Index++)
		{
			HeightsArray.Add(Heights[Index]);
		}

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
						Offset,
						SquareSize,
						SquareSize,
						HeightsArray.GetData(),
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
						Offset,
						SquareSize,
						SquareSize,
						HeightsArray.GetData(),
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
						Offset,
						BicubicSmoothness,
						SquareSize,
						SquareSize,
						HeightsArray.GetData(),
						Iterator.Num(),
						Result.GetData(Iterator));
				}
				break;
				}
			});

		return FVoxelFloatBuffer::Make(Result);
	};
}