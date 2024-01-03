#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelObjectPinType.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelExtraMapNodes.generated.h"

USTRUCT()
struct VOXELEXTRAGRAPHCORE_API FVoxelMap
{
	GENERATED_BODY()

	TVoxelArray<FVoxelRuntimePinValue> Keys;
	TVoxelArray<FVoxelRuntimePinValue> Values;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelMap);

// Given MAP Keys & Values, makes MAP
USTRUCT(Category = "Map", meta = (DisplayName = "Make Map", CompactNodeTitle = "MAP MAKE", Keywords = "map object"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_MapMake : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelWildcardBuffer, Keys, nullptr, ArrayPin);
	VOXEL_INPUT_PIN(FVoxelWildcardBuffer, Values, nullptr, ArrayPin);
	VOXEL_OUTPUT_PIN(FVoxelMap, Result);

	//~ Begin FVoxelNode Interface
#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
	//~ End FVoxelNode Interface
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Get MAP Value
USTRUCT(Category = "Map", meta = (DisplayName = "Get Map Value", CompactNodeTitle = "MAP GET", Keywords = "map object"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_MapGet : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelMap, Map, nullptr);
	VOXEL_INPUT_PIN(FVoxelWildcard, Key, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelWildcard, Value);

	//~ Begin FVoxelNode Interface
#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
	//~ End FVoxelNode Interface
};
