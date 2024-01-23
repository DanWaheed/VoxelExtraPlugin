#include "Data/PCGVoxelData.h"

#include "Data/PCGPointData.h"
#include "VoxelActor.h"
#include "Channel/VoxelWorldChannel.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelRuntimeInfo.h"
#include "VoxelTerminalGraphInstance.h"
#include "VoxelTaskHelpers.h"
#include "VoxelQueryParameter.h"
#include "VoxelPositionQueryParameter.h"
#include "VoxelDependencyTracker.h"

FBox UPCGVoxelData::GetBounds() const
{
	return Bounds;
}

FBox UPCGVoxelData::GetStrictBounds() const
{
	return Bounds;
}

void UPCGVoxelData::Initialize(
	UWorld* InWorld,
	const FName InFloatChannelName,
	const FName InVectorChannelName,
	const bool InSkipVectorChannel,
	const float InGradientStep,
	TSharedPtr<FVoxelRuntimeInfo> InRuntimeInfo,
	const FBox& InBounds)
{
	FloatChannelName = InFloatChannelName;
	VectorChannelName = InVectorChannelName;
	RuntimeInfo = InRuntimeInfo;
	Bounds = InBounds;
	SkipVectorChannel = InSkipVectorChannel;
	GradientStep = InGradientStep;
	World = InWorld;

	FloatChannel = FVoxelWorldChannelManager::FindRuntimeChannel(World, FloatChannelName);
	if (!FloatChannel)
	{
		VOXEL_MESSAGE(Error, "No float channel {0} found. Valid channels: {1}",
			FloatChannelName,
			FVoxelWorldChannelManager::Get(World)->GetValidChannelNames());
		return;
	}

	VectorChannel = FVoxelWorldChannelManager::FindRuntimeChannel(World, VectorChannelName);
	if (!VectorChannel)
	{
		VOXEL_MESSAGE(Error, "No vector channel {0} found. Valid channels: {1}",
			VectorChannelName,
			FVoxelWorldChannelManager::Get(World)->GetValidChannelNames());
		return;
	}
}

bool UPCGVoxelData::SamplePoint(
	const FTransform& InTransform,
	const FBox& InBounds,
	FPCGPoint& OutPoint,
	UPCGMetadata* OutMetadata) const
{
	if (ProjectPoint(InTransform, InBounds, {}, OutPoint, OutMetadata))
	{
		if (InBounds.IsValid)
		{
			return FMath::PointBoxIntersection(OutPoint.Transform.GetLocation(), InBounds.TransformBy(InTransform));
		}
		else
		{
			return (InTransform.GetLocation() - OutPoint.Transform.GetLocation()).SquaredLength() < UE_SMALL_NUMBER;
		}
	}

	return false;
}

bool UPCGVoxelData::ProjectPoint(
	const FTransform& InTransform,
	const FBox& InBounds,
	const FPCGProjectionParams& InParams,
	FPCGPoint& OutPoint,
	UPCGMetadata* OutMetadata) const
{
	FVoxelRuntimePinValue FloatValue = QueryChannel(FloatChannel, InTransform.GetLocation());

	if (!FloatValue.IsValid())
	{
		return false;
	}

	const FVoxelPinType FloatType = FloatValue.GetType().GetExposedType();
	if (FloatType.GetInternalType() != EVoxelPinInternalType::Float) {
		VOXEL_MESSAGE(Error, "Channel returned a value with type {0}, but a float was expected.",
			FloatType.ToString());
		return false;
	}

	FVector4 Slope = OutPoint.Color;
	if (!SkipVectorChannel) 
	{
		FVoxelRuntimePinValue VectorValue = QueryChannel(VectorChannel, InTransform.GetLocation());

		const FVoxelPinType VectorType = VectorValue.GetType().GetExposedType();
		if (VectorType.GetInternalType() != EVoxelPinInternalType::Struct) {
			VOXEL_MESSAGE(Error, "Channel returned a value with type {0}, but a struct was expected.",
				VectorType.ToString());
			return false;
		}

		FVector VectorValueResult = VectorValue.Get<FVector>();
		Slope.X = VectorValueResult.X;
		Slope.Y = VectorValueResult.Y;
		Slope.Z = VectorValueResult.Z;
	}

	float Z = FloatValue.Get<float>();
	FVector Location = InTransform.GetLocation();
	Location.Z = Z;

	OutPoint.Transform.SetIdentity();
	OutPoint.Transform.SetLocation(Location);
	OutPoint.Transform.SetRotation(InTransform.GetRotation());
	OutPoint.Color = Slope;
	OutPoint.Density = 1.f;

	if (!InParams.bProjectRotations)
	{
		OutPoint.Transform.SetRotation(InTransform.GetRotation());
	}
	else
	{
		// Take landscape transform, but respect initial point yaw (don't spin points around Z axis).
		FVector RotVector = InTransform.GetRotation().ToRotationVector();
		RotVector.X = RotVector.Y = 0;
		OutPoint.Transform.SetRotation(OutPoint.Transform.GetRotation() * FQuat::MakeFromRotationVector(RotVector));
	}

	if (!InParams.bProjectScales)
	{
		OutPoint.Transform.SetScale3D(InTransform.GetScale3D());
	}

	return true;
}

UPCGSpatialData* UPCGVoxelData::CopyInternal() const
{
	UPCGVoxelData* NewVoxelData = NewObject<UPCGVoxelData>();

	NewVoxelData->World = World;
	NewVoxelData->FloatChannel = FloatChannel;
	NewVoxelData->VectorChannel = VectorChannel;
	NewVoxelData->Bounds = Bounds;
	NewVoxelData->FloatChannelName = FloatChannelName;
	NewVoxelData->VectorChannelName = VectorChannelName;

	return NewVoxelData;
}

const UPCGPointData* UPCGVoxelData::CreatePointData(FPCGContext* Context, const FBox& InBounds) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UPCGVoxelData::CreatePointData);

	UPCGPointData* Data = NewObject<UPCGPointData>();
	Data->InitializeFromData(this);

	return Data;
}

FVoxelRuntimePinValue UPCGVoxelData::QueryChannel(TSharedPtr<FVoxelRuntimeChannel> Channel, const FVector Position) const
{
	if (!Channel.IsValid()) 
	{
		return {};
	}

	const TSharedRef<FVoxelTerminalGraphInstance> TerminalGraphInstance = FVoxelTerminalGraphInstance::MakeDummy(RuntimeInfo.ToSharedRef());

	FVoxelFloatBufferStorage X; X.Allocate(1);
	FVoxelFloatBufferStorage Y; Y.Allocate(1);
	FVoxelFloatBufferStorage Z; Z.Allocate(1);
	X[0] = Position.X;
	Y[0] = Position.Y;
	Z[0] = Position.Z;

	FString Error;
	FVoxelFutureValue Future;
	const bool bSuccess = FVoxelTaskHelpers::TryRunSynchronouslyGeneric(TerminalGraphInstance, [&]
		{
			const TSharedRef<FVoxelQueryParameters> Parameters = MakeVoxelShared<FVoxelQueryParameters>();
			Parameters->Add<FVoxelPositionQueryParameter>().Initialize(FVoxelVectorBuffer::Make(X, Y, Z));
			Parameters->Add<FVoxelLODQueryParameter>().LOD = 0;
			Parameters->Add<FVoxelGradientStepQueryParameter>().Step = GradientStep;

			const FVoxelQuery Query = FVoxelQuery::Make(
				TerminalGraphInstance,
				Parameters,
				FVoxelDependencyTracker::Create("DependencyTracker"));

			const FVoxelFutureValue FutureValue = Channel->Get(Query);
			if (!ensure(FutureValue.IsValid()))
			{
				return;
			}

			Future =
				MakeVoxelTask()
				.Dependency(FutureValue)
				.Execute(Channel->Definition.Type, [FutureValue]
					{
						return FutureValue.GetValue_CheckCompleted();
					});
		}, &Error);

	if (!bSuccess)
	{
		VOXEL_MESSAGE(Error, "Failed to query value: {0}", Error);
		return {};
	}

	if (!ensure(Future.IsValid()) ||
		!ensure(Future.IsComplete()))
	{
		VOXEL_MESSAGE(Error, "Failed to query value");
		return {};
	}

	FVoxelRuntimePinValue FutureValue = Future.GetValue_CheckCompleted();

	TSharedRef<const FVoxelBuffer> Buffer = FutureValue.GetSharedStruct<FVoxelBuffer>();
	FVoxelRuntimePinValue Constant = Buffer->GetGeneric(0);

	return Constant;
}