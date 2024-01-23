#pragma once

#include "Data/PCGSurfaceData.h"
#include "Channel/VoxelRuntimeChannel.h"
#include "VoxelRuntimeInfo.h"

#include "PCGVoxelData.generated.h"

class UPCGVoxelCache;
class AVoxelActor;
/**
*	Voxel data access abstraction for PCG.
*/
UCLASS(BlueprintType, ClassGroup = (Procedural))
class VOXELPCGEXTENDED_API UPCGVoxelData : public UPCGSpatialDataWithPointCache
{
	GENERATED_BODY()

public:
	void Initialize(
		UWorld* InWorld,
		const FName InFloatChannelName,
		const FName InVectorChannelName,
		const bool InSkipVectorChannel,
		const float InGradientStep,
		TSharedPtr<FVoxelRuntimeInfo> InRuntimeInfo,
		const FBox& InBounds);

	// ~Begin UPCGData interface
	virtual EPCGDataType GetDataType() const override { return EPCGDataType::Surface; }
	// ~End UPCGData interface

	// ~Begin UPGCSpatialData interface
	virtual FBox GetBounds() const override;
	virtual FBox GetStrictBounds() const override;
	virtual int GetDimension() const override { return 0; }
	virtual bool SamplePoint(const FTransform& InTransform, const FBox& InBounds, FPCGPoint& OutPoint, UPCGMetadata* OutMetadata) const override;
	virtual bool ProjectPoint(const FTransform& InTransform, const FBox& InBounds, const FPCGProjectionParams& InParams, FPCGPoint& OutPoint, UPCGMetadata* OutMetadata) const override;

protected:
	virtual UPCGSpatialData* CopyInternal() const override;
	//~End UPCGSpatialData interface

public:
	// ~Begin UPCGSpatialDataWithPointCache interface
	virtual const UPCGPointData* CreatePointData(FPCGContext* Context) const override { return CreatePointData(Context, FBox(EForceInit::ForceInit)); }
	virtual const UPCGPointData* CreatePointData(FPCGContext* Context, const FBox& InBounds) const override;
	// ~End UPCGSpatialDataWithPointCache interface

	TSharedPtr<FVoxelRuntimeChannel> FloatChannel;
	TSharedPtr<FVoxelRuntimeChannel> VectorChannel;
	TSharedPtr<FVoxelRuntimeInfo> RuntimeInfo;
	UWorld* World;

protected:
	UPROPERTY()
	FName FloatChannelName;

	UPROPERTY()
	FName VectorChannelName;

	UPROPERTY()
	bool SkipVectorChannel;

	UPROPERTY()
	float GradientStep;

	UPROPERTY()
	FBox Bounds = FBox(EForceInit::ForceInit);

private:
	FVoxelRuntimePinValue QueryChannel(TSharedPtr<FVoxelRuntimeChannel> Channel, const FVector Position) const;
};