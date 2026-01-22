#include "Animation/AnimNotifyState_DisplacementWarp.h"

#include "CancerAnimationFunctionLibrary.h"
#include "MotionWarp/RootMotionModifier_DisplacementWarp.h"
#include "MotionWarpingComponent.h"
#include "Components/CancerHeroComponent.h"
#include "Components/CancerMotionWarpingComponent.h"
#include "Components/CancerMovementComponent.h"
#include "CancerAbilitySetting.h"
#include "CancerLockingFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotifyState_DisplacementWarp)

static TAutoConsoleVariable<bool> AdsorptionVar(TEXT("Cancer.Adsorption"), false,TEXT(""));

UAnimNotifyState_DisplacementWarp::UAnimNotifyState_DisplacementWarp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootMotionModifier = ObjectInitializer.CreateDefaultSubobject<URootMotionModifier_DisplacementWarp>(
		this, TEXT("RootMotionModifier_DisplacementWarp"));

	//初始化默认参数
	if (URootMotionModifier_DisplacementWarp* Displacement = Cast<URootMotionModifier_DisplacementWarp>(
		RootMotionModifier))
	{
		Displacement->bIgnoreZAxis = bIgnoreZAxis;
		Displacement->bWarpTranslation = bWarpTranslation;
		Displacement->bWarpRotation = bWarpRotation;
		Displacement->bPreserveOriginalRhythm = bPreserveOriginalRhythm;
		Displacement->WarpTargetName = WarpTargetName;
		Displacement->CorrectionTriggerDistance = CorrectionTriggerDistance;
		Displacement->NoTargetDistance = NoTargetDistance;
		Displacement->MaxDisplacementWithTarget = MaxDisplacementWithTarget;
		Displacement->DirectDistanceOverride = DirectDistanceOverride;
		Displacement->FinalOffset = FinalOffset;
	}
}

void UAnimNotifyState_DisplacementWarp::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                    float TotalDuration,
                                                    const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	Debug(MeshComp);
	AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!OwnerActor)
	{
		return;
	}
	UMotionWarpingComponent* MotionWarpingComp = OwnerActor->FindComponentByClass<UMotionWarpingComponent>();
	if (!MotionWarpingComp)
	{
		return;
	}
	MotionWarpingComp->RemoveAllWarpTargets();
	if (WarpTargetName == NAME_None)
	{
		return;
	}
	bool DesiredSprint = false;
	if (auto Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent(OwnerActor))
	{
		DesiredSprint = Movement->IsDesiredSprint();
	}
	if (UCancerMotionWarpingComponent* CancerComp = Cast<UCancerMotionWarpingComponent>(MotionWarpingComp))
	{
		if (auto HeroComponent = UCancerHeroComponent::FindHeroComponent(OwnerActor))
		{
			if (!DesiredSprint)
			{
				if (const AActor* LockedActor = HeroComponent->GetCombatTarget())
				{
					const float MaxClampDistance = MaxDisplacementWithTarget;
					CancerComp->UpdateLocationAndRotation_Character(
						LockedActor, MaxClampDistance, FinalOffset, WarpTargetName, /*bCancelWarpIfCloser*/ true);
					return;
				}
			}
		}
		
		APawn* OwnerPawn = Cast<APawn>(OwnerActor);
		if (bAutoAdsorption&&OwnerPawn->GetController()->IsA(APlayerController::StaticClass()))
		{
			const UWorld* World = OwnerActor->GetWorld();
			if (IsValid(World))
			{
				const FVector OwnerLocation = OwnerActor->GetActorLocation();
				const FRotator OwnerRot = OwnerActor->GetActorRotation();
				const FVector Forward = OwnerRot.Vector();

				FCollisionObjectQueryParams ObjParams;
				if (!AutoAdsorptionChannels.IsEmpty())
				{
					for (const TEnumAsByte<ECollisionChannel>& Channel : AutoAdsorptionChannels)
					{
						if (ObjParams.IsValidObjectQuery(Channel))
						{
							ObjParams.AddObjectTypesToQuery(Channel);
						}
					}
				}
				else
				{
					const auto* Settings = UCancerAbilitySetting::Get();
					if (Settings && !Settings->AutoAdsorptionChannels.IsEmpty())
					{
						for (const TEnumAsByte<ECollisionChannel>& Channel : Settings->AutoAdsorptionChannels)
						{
							if (ObjParams.IsValidObjectQuery(Channel))
							{
								ObjParams.AddObjectTypesToQuery(Channel);
							}
						}
					}
				}

				FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(AutoAdsorption));
				TraceParams.bReturnPhysicalMaterial = false;
				TraceParams.bTraceComplex = false;
				TraceParams.AddIgnoredActor(OwnerActor);

				TArray<FOverlapResult> Overlaps;
				const bool bHasOverlap = World->OverlapMultiByObjectType(
					Overlaps,
					OwnerLocation,
					FQuat::Identity,
					ObjParams,
					FCollisionShape::MakeSphere(FMath::Max(Radius, 0.f)),
					TraceParams
				);

				const AActor* BestActor = nullptr;
				float BestDistSq2D = TNumericLimits<float>::Max();
				if (bHasOverlap)
				{
					const float LifeTime = UCancerAbilitySetting::Get()->MeleeScanDebugDuration;
					for (const FOverlapResult& Overlap : Overlaps)
					{
						AActor* HitActor = Overlap.GetActor();
						if (!IsValid(HitActor) || HitActor == OwnerActor)
						{
							continue;
						}
						const FVector TargetLoc = HitActor->GetActorLocation();
						const FVector ToTarget = TargetLoc - OwnerLocation;
						const FVector ToTarget2D(ToTarget.X, ToTarget.Y, 0.f);
						const FVector Forward2D(Forward.X, Forward.Y, 0.f);
						const float Len2D = ToTarget2D.Size();
						if (Len2D <= KINDA_SMALL_NUMBER)
						{
							continue;
						}
						const float CosTheta = FVector::DotProduct(ToTarget2D.GetSafeNormal(),
						                                           Forward2D.GetSafeNormal());
						const float ThetaDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(CosTheta, -1.f, 1.f)));
						if (ThetaDeg > Angle)
						{
							continue;
						}
						DrawDebugSphere(World, TargetLoc, 10.f, 12, FColor::White, false, LifeTime);
						if (!UCancerLockingFunctionLibrary::IsCanLock(OwnerActor, HitActor))
						{
							continue;
						}
						const float DistSq2D = FVector::DistSquared2D(OwnerLocation, TargetLoc);
						if (DistSq2D < BestDistSq2D)
						{
							BestDistSq2D = DistSq2D;
							BestActor = HitActor;
						}
					}
				}

				if (BestActor)
				{
					const float MaxClampDistance = MaxDisplacementWithTarget;
					CancerComp->UpdateLocationAndRotation_Character(
						BestActor, MaxClampDistance, FinalOffset, WarpTargetName, /*bCancelWarpIfCloser*/ true);
					DrawDebugSphere(World, BestActor->GetActorLocation(), 12.f, 12, FColor::Yellow, false,
					                UCancerAbilitySetting::Get()->MeleeScanDebugDuration);
					return;
				}
			}
		}
	}
}

void UAnimNotifyState_DisplacementWarp::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

void UAnimNotifyState_DisplacementWarp::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                   float FrameDeltaTime,
                                                   const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_DisplacementWarp::Debug(USkeletalMeshComponent* MeshComp)
{
	AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!OwnerActor) return;
	const UWorld* World = OwnerActor->GetWorld();
	if (!IsValid(World)) return;

	const bool bIsGameWorld = World->IsGameWorld();
	const bool bShouldDraw = bIsGameWorld ? AdsorptionVar.GetValueOnGameThread() : bDebug;
	if (!bShouldDraw) return;

	const FVector Loc = bIsGameWorld ? OwnerActor->GetActorLocation() : MeshComp->GetComponentLocation();
	const FVector Fwd = bIsGameWorld ? OwnerActor->GetActorForwardVector() : MeshComp->GetForwardVector();
	const FVector Up = bIsGameWorld ? OwnerActor->GetActorUpVector() : MeshComp->GetUpVector();
	const float HalfH = FMath::Max(Height * 0.5f, 0.f);
	const float AngleRad = FMath::DegreesToRadians(Angle);
	const int32 NumSeg = 24;
	const float LifeTime = UCancerAbilitySetting::Get()->MeleeScanDebugDuration;

	TArray<FVector> TopArc;
	TArray<FVector> BottomArc;
	TopArc.Reserve(NumSeg + 1);
	BottomArc.Reserve(NumSeg + 1);
	for (int32 i = 0; i <= NumSeg; ++i)
	{
		const float T = static_cast<float>(i) / static_cast<float>(NumSeg);
		const float Theta = -AngleRad + 2.f * AngleRad * T;
		const FQuat RotUp = FQuat(Up, Theta);
		const FVector Dir = RotUp.RotateVector(Fwd);
		TopArc.Add(Loc + Dir * FMath::Max(Radius, 0.f) + Up * HalfH);
		BottomArc.Add(Loc + Dir * FMath::Max(Radius, 0.f) - Up * HalfH);
	}

	for (int32 i = 0; i < NumSeg; ++i)
	{
		DrawDebugLine(World, TopArc[i], TopArc[i + 1], FColor::Yellow, false, LifeTime);
		DrawDebugLine(World, BottomArc[i], BottomArc[i + 1], FColor::Yellow, false, LifeTime);
	}

	if (TopArc.Num() > 0)
	{
		DrawDebugLine(World, BottomArc[0], TopArc[0], FColor::Yellow, false, LifeTime);
		DrawDebugLine(World, BottomArc.Last(), TopArc.Last(), FColor::Yellow, false, LifeTime);
		DrawDebugLine(World, Loc - Up * HalfH, BottomArc[0], FColor::Yellow, false, LifeTime);
		DrawDebugLine(World, Loc + Up * HalfH, TopArc[0], FColor::Yellow, false, LifeTime);
		DrawDebugLine(World, Loc - Up * HalfH, BottomArc.Last(), FColor::Yellow, false, LifeTime);
		DrawDebugLine(World, Loc + Up * HalfH, TopArc.Last(), FColor::Yellow, false, LifeTime);
	}
}

URootMotionModifier* UAnimNotifyState_DisplacementWarp::AddRootMotionModifier_Implementation(
	UMotionWarpingComponent* MotionWarpingComp, const UAnimSequenceBase* Animation, float StartTime,
	float EndTime) const
{
	URootMotionModifier_DisplacementWarp* Template = Cast<URootMotionModifier_DisplacementWarp>(RootMotionModifier);
	if (MotionWarpingComp && Template)
	{
		Template->WarpTargetName = WarpTargetName;
		Template->bWarpRotation = bWarpRotation;
		Template->bIgnoreZAxis = bIgnoreZAxis;
		Template->bWarpTranslation = bWarpTranslation;
		Template->CorrectionTriggerDistance = CorrectionTriggerDistance;
		Template->NoTargetDistance = NoTargetDistance;
		Template->MaxDisplacementWithTarget = MaxDisplacementWithTarget;
		Template->DirectDistanceOverride = DirectDistanceOverride;
		Template->FinalOffset = FinalOffset;
		Template->bPreserveOriginalRhythm = bPreserveOriginalRhythm;

		if (WarpTargetName == NAME_None)
		{
			return Super::AddRootMotionModifier_Implementation(MotionWarpingComp, Animation, StartTime, EndTime);
		}

		if (MotionWarpingComp->FindWarpTarget(WarpTargetName))
		{
			return MotionWarpingComp->AddModifierFromTemplate(Template, Animation, StartTime, EndTime);
		}
		Template->bWarpTranslation = false;
		const FTransform TotalRM = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(
			Animation, StartTime, EndTime);
		const FVector TotalT = TotalRM.GetTranslation();
		const float OriginalDistance = bIgnoreZAxis ? TotalT.Size2D() : TotalT.Size();
		const float DesiredDistance = (DirectDistanceOverride > 0.f) ? DirectDistanceOverride : NoTargetDistance;
		if (DesiredDistance > 0.f && OriginalDistance > KINDA_SMALL_NUMBER)
		{
			const float ScaleFactor = DesiredDistance / OriginalDistance;
			const FVector ScaleVec = bIgnoreZAxis ? FVector(ScaleFactor, ScaleFactor, 1.f) : FVector(ScaleFactor);
			URootMotionModifier_Scale::AddRootMotionModifierScale(
				MotionWarpingComp, Animation, StartTime, EndTime, ScaleVec);
		}
		return MotionWarpingComp->AddModifierFromTemplate(Template, Animation, StartTime, EndTime);
	}
	return Super::AddRootMotionModifier_Implementation(MotionWarpingComp, Animation, StartTime, EndTime);
}
