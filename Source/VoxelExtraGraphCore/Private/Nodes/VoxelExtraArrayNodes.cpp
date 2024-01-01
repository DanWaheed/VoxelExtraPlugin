#include "Nodes/VoxelExtraArrayNodes.h"
#include "VoxelBufferUtilities.h"
#include "VoxelBufferBuilder.h"

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

			if (Value < 0 || Value >= NumValues)
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_ArraySetItem, Result)
{
	const TValue<FVoxelBuffer> Values = GetNodeRuntime().Get<FVoxelBuffer>(ValuesPin, Query);
	const TValue<int32> Index = GetNodeRuntime().Get<int32>(IndexPin, Query);
	const FVoxelFutureValue Value = GetNodeRuntime().Get(ValuePin, Query);

	return VOXEL_ON_COMPLETE(Values, Index, Value)
	{
		const int32 NumValues = Values->Num();
		if (Index < 0 || Index >= NumValues)
		{
			VOXEL_MESSAGE(Error, "{0}: Invalid Index {1}. Values.Num={2}", this, Index, NumValues);
			return {};
		}

		const TSharedRef<FVoxelBufferBuilder> BufferBuilder = MakeVoxelShared<FVoxelBufferBuilder>(Values->GetInnerType());
		for (int32 ValueIndex = 0; ValueIndex < NumValues; ValueIndex++)
		{
			if (ValueIndex == Index)
			{
				BufferBuilder->Add(Value);
				continue;
			}

			BufferBuilder->Add(Values->GetGeneric(ValueIndex));
		}

		return FVoxelRuntimePinValue::Make(BufferBuilder->MakeBuffer(), ReturnPinType);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_ArraySetItem::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ValuesPin || Pin.Name == ResultPin)
	{
		return FVoxelPinTypeSet::AllBufferArrays();
	}
	else
	{
		return FVoxelPinTypeSet::AllUniforms();
	}
}

void FVoxelNode_ArraySetItem::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	GetPin(ValuesPin).SetType(NewType.GetBufferType().WithBufferArray(true));
	GetPin(ValuePin).SetType(NewType.GetInnerType());
	GetPin(ResultPin).SetType(NewType.GetBufferType().WithBufferArray(true));
}
#endif