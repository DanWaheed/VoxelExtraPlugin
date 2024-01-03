#include "Nodes/VoxelExtraMapNodes.h"
#include "VoxelBufferUtilities.h"
#include "VoxelBufferBuilder.h"

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
			FVoxelRuntimePinValue Key = Keys->GetGeneric(Index);

			KeysArray[Index] = Key;
		}

		TArray<FVoxelRuntimePinValue> ValuesArray;
		ValuesArray.SetNum(Values->Num());
		for (int32 Index = 0; Index < Values->Num(); Index++)
		{
			FVoxelRuntimePinValue Value = Values->GetGeneric(Index);

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

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MapGet, Value)
{
	const TValue<FVoxelMap> Map = GetNodeRuntime().Get<FVoxelMap>(MapPin, Query);
	const TValue<FVoxelBuffer> Key = GetNodeRuntime().Get<FVoxelBuffer>(KeyPin, Query);
	FVoxelPinType KeyPinType = GetNodeRuntime().GetPinData(KeyPin).Type.GetInnerType();
	FVoxelPinType ValuePinType = GetNodeRuntime().GetPinData(ValuePin).Type.GetInnerType();

	return VOXEL_ON_COMPLETE(Map, Key, KeyPinType, ValuePinType)
	{
		TVoxelArray<FVoxelRuntimePinValue> Keys = Map->Keys;
		TVoxelArray<FVoxelRuntimePinValue> Values = Map->Values;

		if (Values.Num() == 0 || Keys.Num() == 0)
		{
			VOXEL_MESSAGE(Error, "{0}: No keys or values found", this);
			return {};
		}

		FVoxelPinType KeyMapPinType = Keys[0].GetType();
		if (KeyPinType != KeyMapPinType)
		{
			VOXEL_MESSAGE(Error, "{0}: Key Pin Type of {1} Map Key Type of {2}", this, KeyPinType.ToString(), KeyMapPinType.ToString());
			return {};
		}
		
		FVoxelPinType ValueMapPinType = Values[0].GetType();
		if (ValuePinType != ValueMapPinType)
		{
			VOXEL_MESSAGE(Error, "{0}: Value Pin Type of {1} Map Value Type of {2}", this, ValuePinType.ToString(), ValueMapPinType.ToString());
			return {};
		}

		const TSharedRef<FVoxelBufferBuilder> ValueBuilder = MakeVoxelShared<FVoxelBufferBuilder>(ValuePinType);
		for (int32 ValueIndex = 0; ValueIndex < Map->Values.Num(); ValueIndex++)
		{
			ValueBuilder->Add(Map->Values[ValueIndex]);
		}

		FVoxelInt32BufferStorage Indices;
		Indices.Allocate(Key->Num());

		if (Key->IsConstant()) 
		{
			FVoxelRuntimePinValue KeyElement = Key->GetGenericConstant();
			TConstVoxelArrayView<uint8> RawKeyData = KeyElement.GetRawView();

			int32 IndexToUse = -1;
			for (int32 KeyIndex = 0; KeyIndex < Map->Keys.Num(); KeyIndex++)
			{
				FVoxelRuntimePinValue KeyRuntimeValue = Map->Keys[KeyIndex];
				TConstVoxelArrayView<uint8> RawKeyRuntimeData = KeyRuntimeValue.GetRawView();

				if (RawKeyRuntimeData.Num() != RawKeyData.Num())
				{
					continue;
				}

				bool IsSameRawData = true;
				for (int32 RawIndex = 0; RawIndex < RawKeyData.Num(); RawIndex++)
				{
					uint8 DataElement = RawKeyData[RawIndex];
					uint8 RuntimeDataElement = RawKeyRuntimeData[RawIndex];
					if (DataElement != RuntimeDataElement)
					{
						IsSameRawData = false;
						break;
					}
				}

				if (IsSameRawData)
				{
					IndexToUse = KeyIndex;
					break;
				}
			}

			if (IndexToUse == -1) 
			{
				VOXEL_MESSAGE(Error, "{0}: No match found for key", this);
				return {};
			}

			for (int32 Index = 0; Index < Key->Num(); Index++)
			{
				Indices[Index] = IndexToUse;
			}

			const TSharedRef<const FVoxelBuffer> Buffer = FVoxelBufferUtilities::Gather(*ValueBuilder->MakeBuffer(), FVoxelInt32Buffer::Make(Indices));
			return FVoxelRuntimePinValue::Make(Buffer, ReturnPinType);
		}

		for (int32 Index = 0; Index < Key->Num(); Index++)
		{
			FVoxelRuntimePinValue KeyElement = Key->GetGeneric(Index);
			TConstVoxelArrayView<uint8> RawKeyData = KeyElement.GetRawView();

			int32 IndexToUse = -1;
			for (int32 KeyIndex = 0; KeyIndex < Map->Keys.Num(); KeyIndex++) 
			{
				FVoxelRuntimePinValue KeyRuntimeValue = Map->Keys[KeyIndex];
				TConstVoxelArrayView<uint8> RawKeyRuntimeData = KeyRuntimeValue.GetRawView();

				if (RawKeyRuntimeData.Num() != RawKeyData.Num())
				{
					continue;
				}

				bool IsSameRawData = true;
				for (int32 RawIndex = 0; RawIndex < RawKeyData.Num(); RawIndex++)
				{
					uint8 DataElement = RawKeyData[RawIndex];
					uint8 RuntimeDataElement = RawKeyRuntimeData[RawIndex];
					if (DataElement != RuntimeDataElement)
					{
						IsSameRawData = false;
						break;
					}
				}

				if (IsSameRawData)
				{
					IndexToUse = KeyIndex;
					break;
				}
			}

			if (IndexToUse == -1)
			{
				VOXEL_MESSAGE(Error, "{0}: No match found for key", this);
				return {};
			}

			Indices[Index] = IndexToUse;
		}

		const TSharedRef<const FVoxelBuffer> Buffer = FVoxelBufferUtilities::Gather(*ValueBuilder->MakeBuffer(), FVoxelInt32Buffer::Make(Indices));
		return FVoxelRuntimePinValue::Make(Buffer, ReturnPinType);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_MapGet::GetPromotionTypes(const FVoxelPin& Pin) const
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

void FVoxelNode_MapGet::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
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