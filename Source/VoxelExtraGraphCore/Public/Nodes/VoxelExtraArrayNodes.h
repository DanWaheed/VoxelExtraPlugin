#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelExtraArrayNodes.generated.h"

// Given an array and an index, returns the item in the array at that index
USTRUCT(Category = "Array", meta = (DisplayName = "Get Buffered", CompactNodeTitle = "GET", Keywords = "item array"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_ArrayGetItemBuffered : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelWildcardBuffer, Values, nullptr, ArrayPin);
	VOXEL_INPUT_PIN(FVoxelInt32Buffer, Index, 0);
	VOXEL_OUTPUT_PIN(FVoxelWildcardBuffer, Result);

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

// Given an array and an index and a value, sets the item in the array at that index
USTRUCT(Category = "Array", meta = (DisplayName = "Set", CompactNodeTitle = "SET", Keywords = "item array"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_ArraySetItem : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelWildcardBuffer, Values, nullptr, ArrayPin);
	VOXEL_INPUT_PIN(int32, Index, 0);
	VOXEL_INPUT_PIN(FVoxelWildcard, Value, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelWildcardBuffer, Result);

	//~ Begin FVoxelNode Interface
#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
	//~ End FVoxelNode Interface
};