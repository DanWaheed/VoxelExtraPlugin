#include "Nodes/VoxelExtraMapNodes.h"
#include "Utilities/VoxelBufferGatherer.h"
#include "VoxelBufferBuilder.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MapKeys, Keys)
{
	const TValue<FVoxelMap> Map = GetNodeRuntime().Get<FVoxelMap>(MapPin, Query);
	FVoxelPinType OutputPinType = GetNodeRuntime().GetPinData(KeysPin).Type.GetInnerType();

	return VOXEL_ON_COMPLETE(Map, OutputPinType)
	{
		if (const FVoxelPinType KeyMapPinType = Map->Keys[0].GetType(); OutputPinType != KeyMapPinType)
		{
			VOXEL_MESSAGE(Error, "{0}: Key Pin Type of {1} Map Key Type of {2}", this, OutputPinType.ToString(), KeyMapPinType.ToString());
			return {};
		}

		const TSharedRef<FVoxelBufferBuilder> BufferBuilder = MakeVoxelShared<FVoxelBufferBuilder>(OutputPinType);
		for (int32 Index = 0; Index < Map->Keys.Num(); Index++)
		{
			BufferBuilder->Add(Map->Keys[Index]);
		}

		return FVoxelRuntimePinValue::Make(BufferBuilder->MakeBuffer(), ReturnPinType);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_MapKeys::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == KeysPin)
	{
		return FVoxelPinTypeSet::AllBufferArrays();
	}
	else
	{
		return FVoxelPinTypeSet::AllUniforms();
	}
}

void FVoxelNode_MapKeys::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	GetPin(KeysPin).SetType(NewType.GetBufferType().WithBufferArray(true));
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MapValues, Values)
{
	const TValue<FVoxelMap> Map = GetNodeRuntime().Get<FVoxelMap>(MapPin, Query);
	FVoxelPinType OutputPinType = GetNodeRuntime().GetPinData(ValuesPin).Type.GetInnerType();

	return VOXEL_ON_COMPLETE(Map, OutputPinType)
	{
		const FVoxelPinType ValueMapPinType = Map->Values[0].GetType();
		if (OutputPinType != ValueMapPinType)
		{
			VOXEL_MESSAGE(Error, "{0}: Value Pin Type of {1} Map Value Type of {2}", this, OutputPinType.ToString(), ValueMapPinType.ToString());
			return {};
		}

		const TSharedRef<FVoxelBufferBuilder> BufferBuilder = MakeVoxelShared<FVoxelBufferBuilder>(OutputPinType);
		for (int32 Index = 0; Index < Map->Values.Num(); Index++)
		{
			BufferBuilder->Add(Map->Values[Index]);
		}

		return FVoxelRuntimePinValue::Make(BufferBuilder->MakeBuffer(), ReturnPinType);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_MapValues::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ValuesPin)
	{
		return FVoxelPinTypeSet::AllBufferArrays();
	}
	else
	{
		return FVoxelPinTypeSet::AllUniforms();
	}
}

void FVoxelNode_MapValues::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	GetPin(ValuesPin).SetType(NewType.GetBufferType().WithBufferArray(true));
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MapLength, Length)
{
	const TValue<FVoxelMap> Map = GetNodeRuntime().Get<FVoxelMap>(MapPin, Query);

	return VOXEL_ON_COMPLETE(Map)
	{
		return Map->Keys.Num();
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MapMake, Result)
{
	const TValue<FVoxelBuffer> Keys = GetNodeRuntime().Get<FVoxelBuffer>(KeysPin, Query);
	const TValue<FVoxelBuffer> Values = GetNodeRuntime().Get<FVoxelBuffer>(ValuesPin, Query);

	return VOXEL_ON_COMPLETE(Keys, Values)
	{
		int32 KeysCount = Keys->Num();
		int32 ValuesCount = Values->Num();
		if (KeysCount == 0) 
		{
			VOXEL_MESSAGE(Error, "{0}: No Keys Set", this);
			return {};
		}

		if (ValuesCount == 0)
		{
			VOXEL_MESSAGE(Error, "{0}: No Values Set", this);
			return {};
		}

		if (KeysCount != ValuesCount)
		{
			VOXEL_MESSAGE(Error, "{0}: Keys count {1} does not match Values count {2}", this, KeysCount, ValuesCount);
			return {};
		}

		TArray<FVoxelRuntimePinValue> KeysArray;
		KeysArray.SetNum(Keys->Num());
		for (int32 Index = 0; Index < Keys->Num(); Index++)
		{
			const FVoxelRuntimePinValue Key = Keys->GetGeneric(Index);

			KeysArray[Index] = Key;
		}

		TArray<FVoxelRuntimePinValue> ValuesArray;
		ValuesArray.SetNum(Values->Num());
		for (int32 Index = 0; Index < Values->Num(); Index++)
		{
			const FVoxelRuntimePinValue Value = Values->GetGeneric(Index);

			ValuesArray[Index] = Value;
		}

		const TSharedRef<FVoxelMap> Map = MakeVoxelShared<FVoxelMap>();
		Map->Keys = KeysArray;
		Map->Values = ValuesArray;

		return Map;
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_MapMake::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == KeysPin || Pin.Name == ValuesPin)
	{
		return FVoxelPinTypeSet::AllBufferArrays();
	}
	else
	{
		return FVoxelPinTypeSet::AllUniforms();
	}
}

void FVoxelNode_MapMake::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	if (Pin.Name == KeysPin)
	{
		GetPin(KeysPin).SetType(NewType.GetBufferType().WithBufferArray(true));
	}
	else if (Pin.Name == ValuesPin)
	{
		GetPin(ValuesPin).SetType(NewType.GetBufferType().WithBufferArray(true));
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MapFind, Value)
{
	const TValue<FVoxelMap> Map = GetNodeRuntime().Get<FVoxelMap>(MapPin, Query);
	const TValue<FVoxelBuffer> Key = GetNodeRuntime().Get<FVoxelBuffer>(KeyPin, Query);
	FVoxelPinType KeyPinType = GetNodeRuntime().GetPinData(KeyPin).Type.GetInnerType();
	FVoxelPinType ValuePinType = GetNodeRuntime().GetPinData(ValuePin).Type.GetInnerType();

	return VOXEL_ON_COMPLETE(Map, Key, KeyPinType, ValuePinType)
	{
		auto Test = Map;
		TVoxelArray<FVoxelRuntimePinValue> Keys = Map->Keys;
		TVoxelArray<FVoxelRuntimePinValue> Values = Map->Values;

		if (Values.Num() == 0 || Keys.Num() == 0)
		{
			VOXEL_MESSAGE(Error, "{0}: No keys or values found", this);
			return {};
		}

		if (FVoxelPinType KeyMapPinType = Keys[0].GetType(); KeyPinType != KeyMapPinType)
		{
			VOXEL_MESSAGE(Error, "{0}: Key Pin Type of {1} Map Key Type of {2}", this, KeyPinType.ToString(), KeyMapPinType.ToString());
			return {};
		}

		if (FVoxelPinType ValueMapPinType = Values[0].GetType(); ValuePinType != ValueMapPinType)
		{
			VOXEL_MESSAGE(Error, "{0}: Value Pin Type of {1} Map Value Type of {2}", this, ValuePinType.ToString(), ValueMapPinType.ToString());
			return {};
		}

		const TSharedRef<FVoxelBufferBuilder> ValueBuilder = MakeVoxelShared<FVoxelBufferBuilder>(ValuePinType);
		for (int32 ValueIndex = 0; ValueIndex < Values.Num(); ValueIndex++)
		{
			ValueBuilder->Add(Values[ValueIndex]);
		}

		FVoxelInt32BufferStorage Indices;
		Indices.Allocate(Key->Num());

		if (Key->IsConstant()) 
		{
			FVoxelRuntimePinValue KeyElement = Key->GetGenericConstant();
			int32 IndexToUse = FVoxelExtraMapFindProcessor::GetKeyIndex(Keys, KeyElement);
			if (IndexToUse == -1) 
			{
				VOXEL_MESSAGE(Error, "{0}: No match found for key", this);
				return {};
			}

			for (int32 Index = 0; Index < Key->Num(); Index++)
			{
				Indices[Index] = IndexToUse;
			}

			const TSharedRef<const FVoxelBuffer> Buffer = FVoxelBufferGatherer::Gather(*ValueBuilder->MakeBuffer(), FVoxelInt32Buffer::Make(Indices));
			return FVoxelRuntimePinValue::Make(Buffer, ReturnPinType);
		}

		for (int32 Index = 0; Index < Key->Num(); Index++)
		{
			FVoxelRuntimePinValue KeyElement = Key->GetGeneric(Index);
			int32 IndexToUse = FVoxelExtraMapFindProcessor::GetKeyIndex(Keys, KeyElement);

			if (IndexToUse == -1)
			{
				VOXEL_MESSAGE(Error, "{0}: No match found for key", this);
				return {};
			}

			Indices[Index] = IndexToUse;
		}

		const TSharedRef<const FVoxelBuffer> Buffer = FVoxelBufferGatherer::Gather(*ValueBuilder->MakeBuffer(), FVoxelInt32Buffer::Make(Indices));
		return FVoxelRuntimePinValue::Make(Buffer, ReturnPinType);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_MapFind::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == KeyPin || Pin.Name == ValuePin)
	{
		return FVoxelPinTypeSet::AllBuffers();
	}
	else
	{
		return FVoxelPinTypeSet::AllUniforms();
	}
}

void FVoxelNode_MapFind::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	if (Pin.Name == KeyPin) 
	{
		GetPin(KeyPin).SetType(NewType.GetBufferType());
	}
	else if (Pin.Name == ValuePin)
	{
		GetPin(ValuePin).SetType(NewType.GetBufferType());
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MapFindOrDefault, Value)
{
	const TValue<FVoxelMap> Map = GetNodeRuntime().Get<FVoxelMap>(MapPin, Query);
	const TValue<FVoxelBuffer> Key = GetNodeRuntime().Get<FVoxelBuffer>(KeyPin, Query);
	const FVoxelFutureValue Default = GetNodeRuntime().Get(DefaultPin, Query);

	FVoxelPinType KeyPinType = GetNodeRuntime().GetPinData(KeyPin).Type.GetInnerType();
	FVoxelPinType ValuePinType = GetNodeRuntime().GetPinData(ValuePin).Type.GetInnerType();

	return VOXEL_ON_COMPLETE(Map, Key, Default, KeyPinType, ValuePinType)
	{
		auto Test = Map;
		TVoxelArray<FVoxelRuntimePinValue> Keys = Map->Keys;
		TVoxelArray<FVoxelRuntimePinValue> Values = Map->Values;

		if (Values.Num() == 0 || Keys.Num() == 0)
		{
			VOXEL_MESSAGE(Error, "{0}: No keys or values found", this);
			return {};
		}

		if (FVoxelPinType KeyMapPinType = Keys[0].GetType(); KeyPinType != KeyMapPinType)
		{
			VOXEL_MESSAGE(Error, "{0}: Key Pin Type of {1} Map Key Type of {2}", this, KeyPinType.ToString(), KeyMapPinType.ToString());
			return {};
		}

		if (FVoxelPinType ValueMapPinType = Values[0].GetType(); ValuePinType != ValueMapPinType)
		{
			VOXEL_MESSAGE(Error, "{0}: Value Pin Type of {1} Map Value Type of {2}", this, ValuePinType.ToString(), ValueMapPinType.ToString());
			return {};
		}

		const TSharedRef<FVoxelBufferBuilder> ValueBuilder = MakeVoxelShared<FVoxelBufferBuilder>(ValuePinType);
		ValueBuilder->Add(Default);
		for (int32 ValueIndex = 0; ValueIndex < Values.Num(); ValueIndex++)
		{
			ValueBuilder->Add(Values[ValueIndex]);
		}

		FVoxelInt32BufferStorage Indices;
		Indices.Allocate(Key->Num());

		if (Key->IsConstant())
		{
			FVoxelRuntimePinValue KeyElement = Key->GetGenericConstant();
			int32 IndexToUse = FVoxelExtraMapFindProcessor::GetKeyIndex(Keys, KeyElement) + 1;

			for (int32 Index = 0; Index < Key->Num(); Index++)
			{
				Indices[Index] = IndexToUse;
			}

			const TSharedRef<const FVoxelBuffer> Buffer = FVoxelBufferGatherer::Gather(*ValueBuilder->MakeBuffer(), FVoxelInt32Buffer::Make(Indices));
			return FVoxelRuntimePinValue::Make(Buffer, ReturnPinType);
		}

		for (int32 Index = 0; Index < Key->Num(); Index++)
		{
			FVoxelRuntimePinValue KeyElement = Key->GetGeneric(Index);
			int32 IndexToUse = FVoxelExtraMapFindProcessor::GetKeyIndex(Keys, KeyElement) + 1;
			Indices[Index] = IndexToUse;
		}

		const TSharedRef<const FVoxelBuffer> Buffer = FVoxelBufferGatherer::Gather(*ValueBuilder->MakeBuffer(), FVoxelInt32Buffer::Make(Indices));
		return FVoxelRuntimePinValue::Make(Buffer, ReturnPinType);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_MapFindOrDefault::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == KeyPin || Pin.Name == ValuePin)
	{
		return FVoxelPinTypeSet::AllBuffers();
	}
	
	return FVoxelPinTypeSet::AllUniforms();
}

void FVoxelNode_MapFindOrDefault::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	if (Pin.Name == KeyPin)
	{
		GetPin(KeyPin).SetType(NewType.GetBufferType());
	}
	else if (Pin.Name == ValuePin || Pin.Name == DefaultPin)
	{
		GetPin(DefaultPin).SetType(NewType.GetBufferType().GetInnerType());
		GetPin(ValuePin).SetType(NewType.GetBufferType());
	}
}
#endif