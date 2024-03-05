#include "Nodes/VoxelExtraCacheNodes.h"
#include "Utilities/VoxelBufferGatherer.h"
#include "VoxelBufferBuilder.h"

TMap<FName, FVoxelRuntimePinValue> FVoxelNode_CacheArray::NodeCache = TMap<FName, FVoxelRuntimePinValue>();
FCriticalSection FVoxelNode_CacheArray::NodeCacheCriticalSection;

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_CacheArray, Out)
{
	const FName NodeId = GetNodeRuntime().GetNodeRef().NodeId;
	if (NodeCache.Contains(NodeId))
	{
		return VOXEL_ON_COMPLETE(NodeId)
		{
			return NodeCache[NodeId];
		};
	}
	
	const TValue<FVoxelBuffer> InArray = GetNodeRuntime().Get<FVoxelBuffer>(InPin, Query);

	return VOXEL_ON_COMPLETE(InArray, NodeId)
	{
		const FVoxelRuntimePinValue ArrayResultPin = FVoxelRuntimePinValue::Make(InArray, ReturnPinType);
		FScopeLock Lock(&NodeCacheCriticalSection);
		NodeCache.Add(NodeId, ArrayResultPin);
		Lock.Unlock();

		return ArrayResultPin;
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_CacheArray::GetPromotionTypes(const FVoxelPin& Pin) const
{
	return FVoxelPinTypeSet::AllBufferArrays();
}

void FVoxelNode_CacheArray::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	GetPin(InPin).SetType(NewType.GetBufferType().WithBufferArray(true));
	GetPin(OutPin).SetType(NewType.GetBufferType().WithBufferArray(true));
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TMap<FName, FVoxelRuntimePinValue> FVoxelNode_CacheUniform::NodeCache = TMap<FName, FVoxelRuntimePinValue>();
FCriticalSection FVoxelNode_CacheUniform::NodeCacheCriticalSection;

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_CacheUniform, Out)
{
	const FName NodeId = GetNodeRuntime().GetNodeRef().NodeId;
	if (NodeCache.Contains(NodeId))
	{
		return VOXEL_ON_COMPLETE(NodeId)
		{
			return NodeCache[NodeId];
		};
	}
	
	const FVoxelFutureValue In = GetNodeRuntime().Get<FVoxelBuffer>(InPin, Query);

	return VOXEL_ON_COMPLETE(In, NodeId)
	{
		FScopeLock Lock(&NodeCacheCriticalSection);
		NodeCache.Add(NodeId, In);
		Lock.Unlock();

		return In;
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_CacheUniform::GetPromotionTypes(const FVoxelPin& Pin) const
{
	return FVoxelPinTypeSet::AllUniforms();
}

void FVoxelNode_CacheUniform::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	GetPin(InPin).SetType(NewType.GetInnerType());
	GetPin(OutPin).SetType(NewType.GetInnerType());
}
#endif

