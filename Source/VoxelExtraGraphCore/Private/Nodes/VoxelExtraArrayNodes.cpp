#include "Nodes/VoxelExtraArrayNodes.h"
#include "Utilities/VoxelBufferGatherer.h"
#include "VoxelBufferBuilder.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_ArrayMake, Result)
{
	const FVoxelFutureValue Default = GetNodeRuntime().Get(DefaultPin, Query);
	const TValue<int> Size = GetNodeRuntime().Get<int>(SizePin, Query);
	const FVoxelPinType DataType = GetNodeRuntime().GetPinData(DefaultPin).Type;

	const FName NodeId = GetNodeRuntime().GetNodeRef().NodeId;

	return VOXEL_ON_COMPLETE(Default, Size, DataType, NodeId)
	{
		const TSharedRef<FVoxelBufferBuilder> BufferBuilder = MakeVoxelShared<FVoxelBufferBuilder>(DataType);
		for (int32 Index = 0; Index < Size; Index++)
		{
			BufferBuilder->Add(Default);
		}

		const FVoxelRuntimePinValue Result = FVoxelRuntimePinValue::Make(BufferBuilder->MakeBuffer(), ReturnPinType);

		return Result;
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_ArrayMake::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ResultPin)
	{
		return FVoxelPinTypeSet::AllBufferArrays();
	}

	return FVoxelPinTypeSet::AllUniforms();
}

void FVoxelNode_ArrayMake::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	GetPin(DefaultPin).SetType(NewType.GetInnerType());
	GetPin(ResultPin).SetType(NewType.GetBufferType().WithBufferArray(true));

	if (const FVoxelPinType DefaultType = GetPin(DefaultPin).GetType(); DefaultType != NewType)
	{
		GetPin(ResultPin).SetType(DefaultType.GetBufferType().WithBufferArray(true));
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_ArrayGetItemBufferedDefault, Result)
{
	const TValue<FVoxelBuffer> Values = GetNodeRuntime().Get<FVoxelBuffer>(ValuesPin, Query);
	const TValue<FVoxelInt32Buffer> Index = GetNodeRuntime().Get<FVoxelInt32Buffer>(IndexPin, Query);
	const FVoxelFutureValue Default = GetNodeRuntime().Get(DefaultPin, Query);

	FVoxelPinType ValuesPinType = GetNodeRuntime().GetPinData(ValuesPin).Type.GetInnerType();

	return VOXEL_ON_COMPLETE(Values, Index, Default, ValuesPinType)
	{
		const int32 NumValues = Values->Num();
		const int32 IndexSize = Index.Num();

		const TSharedRef<FVoxelBufferBuilder> ValueBuilder = MakeVoxelShared<FVoxelBufferBuilder>(ValuesPinType);
		ValueBuilder->Add(Default);
		for (int32 ValueIndex = 0; ValueIndex < Values->Num(); ValueIndex++)
		{
			ValueBuilder->Add(Values->GetGeneric(ValueIndex));
		}
		
		FVoxelInt32BufferStorage Indices;
		Indices.Allocate(IndexSize);

		for (int32 ValueIndex = 0; ValueIndex < IndexSize; ValueIndex++)
		{
			const int32 Value = Index[ValueIndex];
			if (Value < 0 || Value >= NumValues || !Values->IsValidIndex(Value))
			{
				Indices[ValueIndex] = 0;
				continue;
			}
			
			Indices[ValueIndex] = Value + 1;
		}

		const TSharedRef<const FVoxelBuffer> Buffer = FVoxelBufferGatherer::Gather(*ValueBuilder->MakeBuffer(), FVoxelInt32Buffer::Make(Indices));
		return FVoxelRuntimePinValue::Make(Buffer, ReturnPinType);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_ArrayGetItemBufferedDefault::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ValuesPin)
	{
		return FVoxelPinTypeSet::AllBufferArrays();
	}

	if (Pin.Name == DefaultPin)
	{
		return FVoxelPinTypeSet::AllUniforms();
	}

	return FVoxelPinTypeSet::AllBuffers();
}

void FVoxelNode_ArrayGetItemBufferedDefault::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	GetPin(ValuesPin).SetType(NewType.GetBufferType().WithBufferArray(true));
	GetPin(ResultPin).SetType(NewType.GetInnerType().GetBufferType());
	GetPin(DefaultPin).SetType(NewType.GetBufferType().GetInnerType());

	if (const FVoxelPinType ValuesType = GetPin(ValuesPin).GetType(); ValuesType != NewType) 
	{
		GetPin(ResultPin).SetType(ValuesType.GetInnerType().GetBufferType());
	}
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

	return FVoxelPinTypeSet::AllUniforms();
}

void FVoxelNode_ArraySetItem::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	GetPin(ValuesPin).SetType(NewType.GetBufferType().WithBufferArray(true));
	GetPin(ValuePin).SetType(NewType.GetInnerType());
	GetPin(ResultPin).SetType(NewType.GetBufferType().WithBufferArray(true));
}
#endif