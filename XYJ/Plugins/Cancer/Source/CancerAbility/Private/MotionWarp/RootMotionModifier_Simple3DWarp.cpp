#include "MotionWarp/RootMotionModifier_Simple3DWarp.h"
#include "MotionWarpingComponent.h"
#include "MotionWarpingAdapter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RootMotionModifier_Simple3DWarp)

URootMotionModifier_Simple3DWarp::URootMotionModifier_Simple3DWarp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URootMotionModifier_Simple3DWarp::Update(const FMotionWarpingUpdateContext& Context)
{
	URootMotionModifier::Update(Context);

	UMotionWarpingComponent* OwnerComp = GetOwnerComponent();
	if (!OwnerComp || GetState() != ERootMotionModifierState::Active)
	{
		return;
	}

	const FMotionWarpingTarget* WarpTargetPtr = OwnerComp->FindWarpTarget(WarpTargetName);
	bHasValidTargetCached = (WarpTargetPtr != nullptr);

	FTransform NewTargetTransform = FTransform::Identity;
	if (WarpTargetPtr)
	{
		bRootMotionPaused = WarpTargetPtr->bRootMotionPaused;
		bWarpingPaused = WarpTargetPtr->bWarpingPaused;
		NewTargetTransform = WarpTargetPtr->GetTargetTrasform();
	}
	else
	{
		// 无目标时不更新 NewTargetTransform，由 ProcessRootMotion 判断 bHasValidTargetCached
	}

	if (!CachedTargetTransform.Equals(NewTargetTransform))
	{
		CachedTargetTransform = NewTargetTransform;
		OnTargetTransformChanged();
	}
}

FTransform URootMotionModifier_Simple3DWarp::ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds)
{
	const UMotionWarpingBaseAdapter* OwnerAdapter = GetOwnerAdapter();
	AActor* OwnerActor = OwnerAdapter ? OwnerAdapter->GetActor() : nullptr;
	if (!OwnerAdapter || !OwnerActor)
	{
		return InRootMotion;
	}

	FTransform FinalRootMotion = InRootMotion;

	// 如果没有有效目标，直接返回原始根运动，不进行任何扭曲
	if (!bHasValidTargetCached)
	{
		return InRootMotion;
	}

	if (bWarpTranslation && !bWarpingPaused && !bRootMotionPaused)
	{
		const FVector CurrentLocation = OwnerAdapter->GetVisualRootLocation();
		const FVector TargetLocation = GetTargetLocation();

		// 计算当前时间点在 Warp 窗口中的 Alpha (0-1)
		float Alpha = FMath::Clamp((CurrentPosition - ActualStartTime) / (EndTime - ActualStartTime), 0.f, 1.f);
		
		// 计算下一帧应该到达的位置（线性插值）
		// 注意：ProcessRootMotion 会被多次调用，我们根据 Alpha 计算增量
		float NextAlpha = FMath::Clamp((CurrentPosition + DeltaSeconds - ActualStartTime) / (EndTime - ActualStartTime), 0.f, 1.f);
		
		FVector ExpectedLocation = FMath::Lerp(StartTransform.GetLocation(), TargetLocation, Alpha);
		FVector NextExpectedLocation = FMath::Lerp(StartTransform.GetLocation(), TargetLocation, NextAlpha);
		
		FVector DeltaTranslation = NextExpectedLocation - ExpectedLocation;

		if (bIgnoreZAxis)
		{
			DeltaTranslation.Z = InRootMotion.GetTranslation().Z;
		}

		// 将世界空间的位移增量转回局部空间（因为 RootMotion 是局部空间的）
		FQuat CurrentRotation = OwnerActor->GetActorQuat();
		FinalRootMotion.SetTranslation(CurrentRotation.Inverse().RotateVector(DeltaTranslation));
	}

	if (bWarpRotation && !bWarpingPaused && !bRootMotionPaused)
	{
		// 旋转保持原有的 Warp 逻辑
		const FTransform RootMotionTotal = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Animation.Get(), StartTime, EndTime);
		const FTransform RootMotionDelta = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Animation.Get(), PreviousPosition, FMath::Min(CurrentPosition, EndTime));
		
		FinalRootMotion.SetRotation(WarpRotation(RootMotionDelta, RootMotionTotal, DeltaSeconds));
	}

	return FinalRootMotion;
}

URootMotionModifier_Simple3DWarp* URootMotionModifier_Simple3DWarp::AddRootMotionModifierSimple3DWarp(
	UMotionWarpingComponent* InMotionWarpingComp,
	const UAnimSequenceBase* InAnimation,
	float InStartTime,
	float InEndTime,
	FName InWarpTargetName,
	bool bInWarpTranslation,
	bool bInIgnoreZAxis,
	bool bInWarpRotation)
{
	if (ensureAlways(InMotionWarpingComp))
	{
		URootMotionModifier_Simple3DWarp* NewModifier = NewObject<URootMotionModifier_Simple3DWarp>(InMotionWarpingComp);
		NewModifier->Animation = InAnimation;
		NewModifier->StartTime = InStartTime;
		NewModifier->EndTime = InEndTime;
		NewModifier->WarpTargetName = InWarpTargetName;
		NewModifier->bWarpTranslation = bInWarpTranslation;
		NewModifier->bIgnoreZAxis = bInIgnoreZAxis;
		NewModifier->bWarpRotation = bInWarpRotation;

		InMotionWarpingComp->AddModifier(NewModifier);
		return NewModifier;
	}
	return nullptr;
}
