#pragma once

#include "Elements/PCGDataFromActor.h"
#include "Channel/VoxelChannelName.h"

#include "VoxelPCGActorGetter.generated.h"

/** Builds a collection of landscape data from the selected actors. */
UCLASS(BlueprintType, ClassGroup = (Procedural))
class VOXELPCGEXTENDED_API UPCGGetVoxelChannelSettings : public UPCGSettings
{
	GENERATED_BODY()

public:
	UPCGGetVoxelChannelSettings();

	//~Begin UPCGSettings interface
#if WITH_EDITOR
	virtual FName GetDefaultNodeName() const override { return FName(TEXT("GetVoxelChannelData")); }
	virtual FText GetDefaultNodeTitle() const override { return NSLOCTEXT("PCGGetVoxelSettings", "NodeTitle", "Get Voxel Channel Data"); }
	virtual FText GetNodeTooltipText() const override;
#endif

	virtual FName AdditionalTaskName() const override;

protected:
	virtual TArray<FPCGPinProperties> OutputPinProperties() const override;

	virtual FPCGElementPtr CreateElement() const override;
	//~End UPCGSettings

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings", meta = (PCG_Overridable))
	FVoxelChannelName FloatChannel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings", meta = (PCG_Overridable))
	FVoxelChannelName VectorChannel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings", meta = (PCG_Overridable))
	bool SkipVectorChannel;
};

class FVoxelChannelPCGContext : public FPCGContext
{
public:
	TSharedPtr<FVoxelRuntimeChannel> FloatChannel;
	TSharedPtr<FVoxelRuntimeChannel> VectorChannel;
	bool bPerformedQuery = false;
};

class FPCGGetVoxelChannelDataElement : public IPCGElement
{
public:
	virtual FPCGContext* Initialize(const FPCGDataCollection& InputData, TWeakObjectPtr<UPCGComponent> SourceComponent, const UPCGNode* Node) override;
	virtual bool CanExecuteOnlyOnMainThread(FPCGContext* Context) const override { return true; }
	virtual bool IsCacheable(const UPCGSettings* InSettings) const override { return false; }

protected:
	virtual bool ExecuteInternal(FPCGContext* InContext) const override;
};