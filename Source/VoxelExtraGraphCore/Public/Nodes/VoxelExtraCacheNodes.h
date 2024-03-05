#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelExtraCacheNodes.generated.h"

// Cache an Array
USTRUCT(Category = "Cache", meta = (DisplayName = "Cache Array", CompactNodeTitle = "CACHE ARRAY", Keywords = "cache save store array"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_CacheArray : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelWildcardBuffer, In, nullptr, ArrayPin);
	VOXEL_OUTPUT_PIN(FVoxelWildcardBuffer, Out);

	//~ Begin FVoxelNode Interface
#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
	//~ End FVoxelNode Interface

	static TMap<FName, FVoxelRuntimePinValue> NodeCache;
	static FCriticalSection NodeCacheCriticalSection;
};

// Cache a uniform
USTRUCT(Category = "Cache", meta = (DisplayName = "Cache Uniform", CompactNodeTitle = "CACHE Uniform", Keywords = "cache save store uniform"))
struct VOXELEXTRAGRAPHCORE_API FVoxelNode_CacheUniform : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelWildcard, In, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelWildcard, Out);

	//~ Begin FVoxelNode Interface
#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
	//~ End FVoxelNode Interface

	static TMap<FName, FVoxelRuntimePinValue> NodeCache;
	static FCriticalSection NodeCacheCriticalSection;
};