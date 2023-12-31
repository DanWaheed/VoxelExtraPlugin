#include "Nodes/VoxelExtraArrayNodes.h"
#include "VoxelBufferUtilities.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_ArrayGetItemBuffered, Result)
{
	const TValue<FVoxelBuffer> Values = GetNodeRuntime().Get<FVoxelBuffer>(ValuesPin, Query);
	const TValue<FVoxelInt32Buffer> Index = GetNodeRuntime().Get<FVoxelInt32Buffer>(IndexPin, Query);

	return VOXEL_ON_COMPLETE(Values, Index)
	{
		const int32 NumValues = Values->Num();
		const int32 IndexSize = Index.Num();
	
		FVoxelInt32BufferStorage Indices;
		Indices.Allocate(IndexSize);

		for (int32 ValueIndex = 0; ValueIndex < IndexSize; ValueIndex++)
		{
			int32 Value = Index[ValueIndex];

			if (Value < 0 ||
				Value >= NumValues)
			{
				Indices[ValueIndex] = 0;
				continue;
			}

			Indices[ValueIndex] = Value;
		}

		const TSharedRef<const FVoxelBuffer> Buffer = FVoxelBufferUtilities::Gather(*Values, FVoxelInt32Buffer::Make(Indices));
		return FVoxelRuntimePinValue::Make(Buffer, ReturnPinType);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_ArrayGetItemBuffered::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ValuesPin)
	{
		return FVoxelPinTypeSet::AllBufferArrays();
	}
	else
	{
		return FVoxelPinTypeSet::AllBuffers();
	}
}

void FVoxelNode_ArrayGetItemBuffered::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	GetPin(ValuesPin).SetType(NewType.GetBufferType().WithBufferArray(true));
	GetPin(ResultPin).SetType(NewType.GetInnerType().GetBufferType());
}
#endif