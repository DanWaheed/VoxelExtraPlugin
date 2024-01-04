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
USTRUCT(Category = "Map", meta = (DisplayName = "Make Map", CompactNodeTitle = "MAKE", Keywords = "map object"))
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

// Find MAP Value
USTRUCT(Category = "Map", meta = (DisplayName = "Find Map Value", CompactNodeTitle = "FIND", Keywords = "map object"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_MapFind : public FVoxelNode
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Get MAP Keys
USTRUCT(Category = "Map", meta = (DisplayName = "Get Map Keys", CompactNodeTitle = "KEYS", Keywords = "map object"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_MapKeys : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelMap, Map, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelWildcardBuffer, Keys);

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

// Get MAP Values
USTRUCT(Category = "Map", meta = (DisplayName = "Get Map Values", CompactNodeTitle = "VALUES", Keywords = "map object"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_MapValues : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelMap, Map, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelWildcardBuffer, Values);

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

// Get MAP Length
USTRUCT(Category = "Map", meta = (DisplayName = "Get Map Length", CompactNodeTitle = "LENGTH", Keywords = "map object"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_MapLength : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelMap, Map, nullptr);
	VOXEL_OUTPUT_PIN(int32, Length);
};