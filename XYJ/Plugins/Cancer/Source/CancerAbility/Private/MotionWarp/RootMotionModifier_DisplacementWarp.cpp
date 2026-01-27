#include "MotionWarp/RootMotionModifier_DisplacementWarp.h"
#include "MotionWarpingComponent.h"
#include "MotionWarpingAdapter.h"
#include "DrawDebugHelpers.h"
#include "Components/CancerMotionWarpingComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RootMotionModifier_DisplacementWarp)

URootMotionModifier_DisplacementWarp::URootMotionModifier_DisplacementWarp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URootMotionModifier_DisplacementWarp::Update(const FMotionWarpingUpdateContext& Context)
{
	// 执行基础更新（时间与状态）
	URootMotionModifier::Update(Context);

	UMotionWarpingComponent* OwnerComp = GetOwnerComponent();
	if (!OwnerComp || GetState() != ERootMotionModifierState::Active)
	{
		return;
	}
	const FMotionWarpingTarget* WarpTargetPtr = OwnerComp->FindWarpTarget(WarpTargetName);
	bHasValidTargetCached = (WarpTargetPtr != nullptr);

	// 若存在目标，按父类逻辑计算目标变换与动画偏移
	FTransform NewTargetTransform = FTransform::Identity;
	if (WarpTargetPtr)
	{
		bRootMotionPaused = WarpTargetPtr->bRootMotionPaused;
		bWarpingPaused = WarpTargetPtr->bWarpingPaused;

		// 游戏侧目标
		FTransform WarpPointTransformGame = WarpTargetPtr->GetTargetTrasform();
		NewTargetTransform = WarpPointTransformGame;

		// 动画内置 warp 点偏移
		if (WarpPointAnimProvider != EWarpPointAnimProvider::None)
		{
			if (!CachedOffsetFromWarpPoint.IsSet())
			{
				if (const UMotionWarpingBaseAdapter* OwnerAdapter = GetOwnerAdapter())
				{
					if (WarpPointAnimProvider == EWarpPointAnimProvider::Static)
					{
						CachedOffsetFromWarpPoint =
							UMotionWarpingUtilities::CalculateRootTransformRelativeToWarpPointAtTime(
								*OwnerAdapter, GetAnimation(), EndTime, WarpPointAnimTransform);
					}
					else if (WarpPointAnimProvider == EWarpPointAnimProvider::Bone)
					{
						CachedOffsetFromWarpPoint =
							UMotionWarpingUtilities::CalculateRootTransformRelativeToWarpPointAtTime(
								*OwnerAdapter, GetAnimation(), EndTime, WarpPointAnimBoneName);
					}
				}
			}

			NewTargetTransform = CachedOffsetFromWarpPoint.GetValue() * WarpPointTransformGame;
		}
	}
	else
	{
		const float Desired2D = (DirectDistanceOverride > 0.f) ? DirectDistanceOverride : NoTargetDistance;

		const UMotionWarpingBaseAdapter* OwnerAdapterLocal = GetOwnerAdapter();
		const AActor* OwnerActorLocal = OwnerAdapterLocal ? OwnerAdapterLocal->GetActor() : nullptr;

		const FVector BaseLoc = OwnerActorLocal ? OwnerActorLocal->GetActorLocation() : StartTransform.GetLocation();
		const float BaseYaw = OwnerActorLocal
			                      ? OwnerActorLocal->GetActorRotation().Yaw
			                      : StartTransform.GetRotation().Rotator().Yaw;
		const FRotator YawRot(0.f, BaseYaw, 0.f);
		const FVector ForwardYaw = YawRot.Vector();

		FVector PseudoTarget = BaseLoc + ForwardYaw * FMath::Max(Desired2D, 0.f);
		if (bIgnoreZAxis)
		{
			PseudoTarget.Z = BaseLoc.Z;
		}
		NewTargetTransform = FTransform(YawRot.Quaternion(), PseudoTarget);
	}

	if (!CachedTargetTransform.Equals(NewTargetTransform))
	{
		CachedTargetTransform = NewTargetTransform;
		OnTargetTransformChanged();
	}
	if (bHasValidTargetCached && CorrectionTriggerDistance > 0.f)
	{
		const UMotionWarpingBaseAdapter* OwnerAdapter = GetOwnerAdapter();
		const AActor* OwnerActor = OwnerAdapter ? OwnerAdapter->GetActor() : nullptr;
		if (OwnerActor)
		{
			const FVector OwnerLoc = OwnerActor->GetActorLocation();
			const FVector TargetLoc = NewTargetTransform.GetLocation();
			const float Dist2D = FVector::Dist2D(OwnerLoc, TargetLoc);
			if (Dist2D > CorrectionTriggerDistance)
			{
				bHasValidTargetCached = false;
			}
		}
	}
}

FTransform URootMotionModifier_DisplacementWarp::ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds)
{
	const UMotionWarpingBaseAdapter* OwnerAdapter = GetOwnerAdapter();
	AActor* OwnerActor = OwnerAdapter ? OwnerAdapter->GetActor() : nullptr;
	if (!OwnerAdapter || !OwnerActor)
	{
		return InRootMotion;
	}

	FTransform FinalRootMotion = InRootMotion;

	const FTransform RootMotionTotal = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(
		Animation.Get(), PreviousPosition, EndTime);
	const FTransform RootMotionDelta = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(
		Animation.Get(), PreviousPosition, FMath::Min(CurrentPosition, EndTime));

	FTransform ExtraRootMotion = FTransform::Identity;
	if (CurrentPosition > EndTime)
	{
		ExtraRootMotion = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(
			Animation.Get(), EndTime, CurrentPosition);
	}

	if (bWarpTranslation)
	{
		if (bRootMotionPaused)
		{
			FinalRootMotion.SetTranslation(FVector::ZeroVector);
		}
		else if (!bWarpingPaused)
		{
			const FVector CurrentLocation = OwnerAdapter->GetVisualRootLocation();
			const FQuat CurrentRotation = OwnerActor->GetActorQuat();
			FVector MeshTranslationOffset = OwnerAdapter->GetBaseVisualTranslationOffset();
			if (!bWarpToFeetLocation)
			{
				MeshTranslationOffset.Z = 0.f;
			}
			const FQuat MeshRotationOffset = OwnerAdapter->GetBaseVisualRotationOffset();

			const FVector DeltaTranslation = RootMotionDelta.GetTranslation();
			const FVector TotalTranslation = RootMotionTotal.GetTranslation();

			// 有锁定目标：直接使用组件写入的目标位置与旋转
			FVector TargetLocation = GetTargetLocation();
			if (bIgnoreZAxis)
			{
				TargetLocation.Z = CurrentLocation.Z;
			}

			const FTransform MeshTransform = FTransform(MeshRotationOffset, MeshTranslationOffset) * OwnerActor->
				GetActorTransform();

			// 有原始位移：根据选择策略进行处理
			const bool bHasOriginalTranslation = !TotalRootMotionWithinWindow.GetTranslation().IsNearlyZero();
			if (bHasOriginalTranslation && bPreserveOriginalRhythm)
			{
				// 保留节奏：按比例缩放每帧的位移
				const float HorizontalDelta = RootMotionDelta.GetTranslation().Size2D();
				const float HorizontalOriginal = RootMotionTotal.GetTranslation().Size2D();

				// 期望总水平距离：有目标时直接使用到目标的距离；
				// 无目标时才使用覆盖/最大距离逻辑
				float DesiredHorizontalTarget = FVector::Dist2D(CurrentLocation, TargetLocation);
				if (!bHasValidTargetCached)
				{
					if (DirectDistanceOverride > 0.f)
					{
						DesiredHorizontalTarget = DirectDistanceOverride;
					}
					else if (MaxDisplacementWithTarget > 0.f)
					{
						DesiredHorizontalTarget = FMath::Min(DesiredHorizontalTarget, MaxDisplacementWithTarget);
					}
				}

				const float HorizontalTranslationWarped = !FMath::IsNearlyZero(HorizontalOriginal)
					                                          ? ((HorizontalDelta * DesiredHorizontalTarget) /
						                                          HorizontalOriginal)
					                                          : 0.f;

				FVector LocalToTarget = MeshTransform.InverseTransformPositionNoScale(TargetLocation);
				if (bSubtractRemainingRootMotion)
				{
					LocalToTarget -= RootMotionRemainingAfterNotify.GetTranslation();
				}

				FVector WarpedDelta = LocalToTarget.GetSafeNormal2D() * HorizontalTranslationWarped;

				if (!bIgnoreZAxis)
				{
					const float VerticalDelta = RootMotionDelta.GetTranslation().Z;
					const float VerticalOriginal = RootMotionTotal.GetTranslation().Z;
					//const float CapsuleHalfHeight = OwnerAdapter->GetCapsuleHalfHeight();
					const FVector CapsuleBottomLocation = (OwnerActor->GetActorLocation()
						/*- FVector(0.f, 0.f, CapsuleHalfHeight)*/);
					float DesiredVerticalTarget = TargetLocation.Z - CapsuleBottomLocation.Z;
					float VerticalTranslationWarped = 0.f;
					if (!FMath::IsNearlyZero(VerticalOriginal))
					{
						VerticalTranslationWarped = (VerticalDelta * DesiredVerticalTarget) / VerticalOriginal;
					}
					else if (!FMath::IsNearlyZero(HorizontalOriginal))
					{
						// 如果动画没有垂直位移，但有水平位移，则按水平位移的比例来分配垂直位移
						VerticalTranslationWarped = (HorizontalDelta * DesiredVerticalTarget) / HorizontalOriginal;
					}
					else
					{
						// 如果连水平位移都没有，则按时间线性分配
						float TimeAlpha = (CurrentPosition - PreviousPosition) / (EndTime - PreviousPosition);
						VerticalTranslationWarped = DesiredVerticalTarget * TimeAlpha;
					}
					WarpedDelta.Z = VerticalTranslationWarped;
				}
				else
				{
					WarpedDelta.Z = InRootMotion.GetTranslation().Z;
				}

				FinalRootMotion.SetTranslation(WarpedDelta + ExtraRootMotion.GetLocation());
			}
			else
			{
				// 均匀位移：按时间插值当前位置到目标位置，得到恒定步长
				float Alpha = FMath::Clamp((CurrentPosition - ActualStartTime) / (EndTime - ActualStartTime), 0.f, 1.f);
				Alpha = FAlphaBlend::AlphaToBlendOption(Alpha, AddTranslationEasingFunc, AddTranslationEasingCurve);

				// 均匀位移：直接插值到组件提供的目标位置
				FVector NextLocation = FMath::Lerp<FVector, float>(StartTransform.GetLocation(), TargetLocation, Alpha);
				if (bIgnoreZAxis)
				{
					NextLocation.Z = CurrentLocation.Z;
				}

				const FVector DeltaToTarget = TargetLocation - CurrentLocation;
				FVector FinalDeltaTranslation = (NextLocation - CurrentLocation);
				FinalDeltaTranslation = (CurrentRotation.Inverse() * DeltaToTarget.ToOrientationQuat()).
					GetForwardVector() * FinalDeltaTranslation.Size();
				FinalDeltaTranslation = MeshRotationOffset.UnrotateVector(FinalDeltaTranslation);

				FinalRootMotion.SetTranslation(FinalDeltaTranslation + ExtraRootMotion.GetLocation());
			}
		}
	}

	if (bWarpRotation)
	{
		const FQuat WarpedRotation = ExtraRootMotion.GetRotation() * WarpRotation(
			RootMotionDelta, RootMotionTotal, DeltaSeconds);
		FinalRootMotion.SetRotation(WarpedRotation);
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	const int32 DebugLevel = FMotionWarpingCVars::CVarMotionWarpingDebug.GetValueOnGameThread();
	if (DebugLevel == 1 || DebugLevel == 3)
	{
		PrintLog(TEXT("DisplacementWarp"), InRootMotion, FinalRootMotion);
	}
	if (DebugLevel == 2 || DebugLevel == 3)
	{
		const float DrawDebugDuration = FMotionWarpingCVars::CVarMotionWarpingDrawDebugDuration.GetValueOnGameThread();
		DrawDebugCoordinateSystem(OwnerActor->GetWorld(), GetTargetLocation(), GetTargetRotator(), 50.f, false,
		                          DrawDebugDuration, 0, 1.f);
	}
#endif

	return FinalRootMotion;
}

URootMotionModifier_DisplacementWarp* URootMotionModifier_DisplacementWarp::AddRootMotionModifierDisplacementWarp(
	UMotionWarpingComponent* InMotionWarpingComp,
	const UAnimSequenceBase* InAnimation,
	float InStartTime,
	float InEndTime,
	FName InWarpTargetName,
	EWarpPointAnimProvider InWarpPointAnimProvider,
	FTransform InWarpPointAnimTransform,
	FName InWarpPointAnimBoneName,
	bool bInWarpTranslation,
	bool bInIgnoreZAxis,
	bool bInWarpRotation,
	EMotionWarpRotationType InRotationType,
	EMotionWarpRotationMethod InRotationMethod,
	float InWarpRotationTimeMultiplier,
	float InWarpMaxRotationRate,
	bool bInPreserveOriginalRhythm,
	float InNoTargetDistance,
	float InMaxDisplacementWithTarget,
	float InDirectDistanceOverride,
	FVector InFinalOffset)
{
	if (ensureAlways(InMotionWarpingComp))
	{
		URootMotionModifier_DisplacementWarp* NewModifier = NewObject<URootMotionModifier_DisplacementWarp>(
			InMotionWarpingComp);
		NewModifier->Animation = InAnimation;
		NewModifier->StartTime = InStartTime;
		NewModifier->EndTime = InEndTime;
		NewModifier->WarpTargetName = InWarpTargetName;
		NewModifier->WarpPointAnimProvider = InWarpPointAnimProvider;
		NewModifier->WarpPointAnimTransform = InWarpPointAnimTransform;
		NewModifier->WarpPointAnimBoneName = InWarpPointAnimBoneName;
		NewModifier->bWarpTranslation = bInWarpTranslation;
		NewModifier->bIgnoreZAxis = bInIgnoreZAxis;
		NewModifier->bWarpRotation = bInWarpRotation;
		NewModifier->RotationType = InRotationType;
		NewModifier->RotationMethod = InRotationMethod;
		NewModifier->WarpRotationTimeMultiplier = InWarpRotationTimeMultiplier;
		NewModifier->WarpMaxRotationRate = InWarpMaxRotationRate;

		NewModifier->bPreserveOriginalRhythm = bInPreserveOriginalRhythm;
		NewModifier->NoTargetDistance = InNoTargetDistance;
		NewModifier->MaxDisplacementWithTarget = InMaxDisplacementWithTarget;
		NewModifier->DirectDistanceOverride = InDirectDistanceOverride;
		NewModifier->FinalOffset = InFinalOffset;

		InMotionWarpingComp->AddModifier(NewModifier);
		return NewModifier;
	}
	return nullptr;
}
