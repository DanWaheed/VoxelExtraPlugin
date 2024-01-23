#include "Elements/VoxelPCGActorGetter.h"

#include "PCGComponent.h"
#include "Data/PCGVoxelData.h"
#include "Helpers/PCGHelpers.h"
#include "VoxelRuntimeInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VoxelPCGActorGetter)

#define LOCTEXT_NAMESPACE "PCGGetVoxelChannel"

UPCGGetVoxelChannelSettings::UPCGGetVoxelChannelSettings()
{
	// Nope
}

TArray<FPCGPinProperties> UPCGGetVoxelChannelSettings::OutputPinProperties() const
{
	TArray<FPCGPinProperties> PinProperties;
	PinProperties.Emplace(PCGPinConstants::DefaultOutputLabel, EPCGDataType::Surface, /*bAllowMultipleConnections=*/true, /*bAllowMultipleData=*/true);

	return PinProperties;
}

FName UPCGGetVoxelChannelSettings::AdditionalTaskName() const
{
	// Do not use the version from data from actor otherwise we'll show the selected actor class, which serves no purpose
	return UPCGSettings::AdditionalTaskName();
}

#if WITH_EDITOR
FText UPCGGetVoxelChannelSettings::GetNodeTooltipText() const
{
	return LOCTEXT("GetVoxelTooltip", "Builds a collection of voxel actors from the selected actors.");
}
#endif

FPCGElementPtr UPCGGetVoxelChannelSettings::CreateElement() const
{
	return MakeShared<FPCGGetVoxelChannelDataElement>();
}

FPCGContext* FPCGGetVoxelChannelDataElement::Initialize(
	const FPCGDataCollection& InputData,
	TWeakObjectPtr<UPCGComponent> SourceComponent,
	const UPCGNode* Node)
{
	FVoxelChannelPCGContext* Context = new FVoxelChannelPCGContext();
	Context->InputData = InputData;
	Context->SourceComponent = SourceComponent;
	Context->Node = Node;

	return Context;
}

bool FPCGGetVoxelChannelDataElement::ExecuteInternal(FPCGContext* InContext) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FPCGGetVoxelChannelDataElement::Execute);

	check(InContext);
	FVoxelChannelPCGContext* Context = static_cast<FVoxelChannelPCGContext*>(InContext);

	const UPCGGetVoxelChannelSettings* Settings = Context->GetInputSettings<UPCGGetVoxelChannelSettings>();
	check(Settings);

	if (!Context->bPerformedQuery)
	{
		const UPCGComponent* PCGComponent = Context->SourceComponent.IsValid() ? Context->SourceComponent.Get() : nullptr;
		const AActor* Self = PCGComponent ? PCGComponent->GetOwner() : nullptr;
		FBox ActorBounds = FBox(EForceInit::ForceInit);
		if (Self)
		{
			// Capture ActorBounds by value because it goes out of scope
			ActorBounds = PCGHelpers::GetActorBounds(Self);
		}

		UWorld* World = PCGComponent ? PCGComponent->GetWorld() : nullptr;

		const TSharedRef<FVoxelRuntimeInfo> RuntimeInfo =
			FVoxelRuntimeInfoBase::MakeFromWorld(World)
			.EnableParallelTasks()
			.MakeRuntimeInfo_RequiresDestroy();

		UPCGVoxelData* VoxelData = NewObject<UPCGVoxelData>();
		VoxelData->Initialize(
			World,
			Settings->FloatChannel.Name,
			Settings->VectorChannel.Name,
			Settings->SkipVectorChannel,
			Settings->GradientStep,
			RuntimeInfo.ToSharedPtr(),
			ActorBounds);

		Context->FloatChannel = VoxelData->FloatChannel;
		Context->VectorChannel = VoxelData->VectorChannel;

		FPCGTaggedData& TaggedData = InContext->OutputData.TaggedData.Emplace_GetRef();
		TaggedData.Data = VoxelData;

		Context->bPerformedQuery = true;
	}

	return true;
}

#undef LOCTEXT_NAMESPACE
