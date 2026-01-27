#include "Animation/AnimNotifyState_TargetSectorDetection.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerAbilitySetting.h"
#include "CancerLockingFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotifyState_TargetSectorDetection)

UAnimNotifyState_TargetSectorDetection::UAnimNotifyState_TargetSectorDetection()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 100, 100);
#endif
}

void UAnimNotifyState_TargetSectorDetection::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                        float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AffectedActors.Empty();
}

void UAnimNotifyState_TargetSectorDetection::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                       float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (bDebug)
	{
		DrawDebug(MeshComp);
	}

	AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!OwnerActor) return;

	const UWorld* World = OwnerActor->GetWorld();
	if (!IsValid(World) || !World->IsGameWorld()) return;

	FVector SearchOrigin;
	FVector Forward;
	GetSearchInfo(MeshComp, SearchOrigin, Forward);

	FCollisionObjectQueryParams ObjParams;
	if (!DetectionChannels.IsEmpty())
	{
		for (const TEnumAsByte<ECollisionChannel>& Channel : DetectionChannels)
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

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(TargetSectorDetection));
	TraceParams.AddIgnoredActor(OwnerActor);

	TArray<FOverlapResult> Overlaps;
	const bool bHasOverlap = World->OverlapMultiByObjectType(
		Overlaps,
		SearchOrigin,
		FQuat::Identity,
		ObjParams,
		FCollisionShape::MakeSphere(FMath::Max(Radius, 0.f)),
		TraceParams
	);

	if (bHasOverlap)
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			AActor* HitActor = Overlap.GetActor();
			if (!IsValid(HitActor) || HitActor == OwnerActor) continue;

			// 检查是否已经在处理列表中（避免重复添加标签）
			if (AffectedActors.Contains(HitActor)) continue;

			const FVector TargetLoc = HitActor->GetActorLocation();
			const FVector ToTarget = TargetLoc - SearchOrigin;
			const FVector ToTarget2D(ToTarget.X, ToTarget.Y, 0.f);
			const FVector Forward2D(Forward.X, Forward.Y, 0.f);
			
			const float Len2D = ToTarget2D.Size();
			if (Len2D <= KINDA_SMALL_NUMBER) continue;

			const float CosTheta = FVector::DotProduct(ToTarget2D.GetSafeNormal(), Forward2D.GetSafeNormal());
			const float ThetaDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(CosTheta, -1.f, 1.f)));

			if (ThetaDeg <= Angle)
			{
				// 高度检查
				if (FMath::Abs(TargetLoc.Z - SearchOrigin.Z) <= Height * 0.5f)
				{
					if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor))
					{
						AffectedActors.Add(HitActor);
					}
				}
			}
		}
	}
}

void UAnimNotifyState_TargetSectorDetection::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                      const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	AffectedActors.Empty();
}

void UAnimNotifyState_TargetSectorDetection::GetSearchInfo(USkeletalMeshComponent* MeshComp, FVector& OutLocation,
                                                          FVector& OutForward) const
{
	if (!MeshComp) return;
	
	AActor* OwnerActor = MeshComp->GetOwner();
	const UWorld* World = MeshComp->GetWorld();
	const bool bIsGameWorld = World && World->IsGameWorld();

	if (bIsGameWorld && OwnerActor)
	{
		OutLocation = OwnerActor->GetActorLocation();
		const FRotator OwnerRot = OwnerActor->GetActorRotation();
		OutForward = OwnerRot.Vector();
		OutLocation += OwnerRot.RotateVector(DetectionOffset);
	}
	else
	{
		OutLocation = MeshComp->GetComponentLocation();
		if (MeshComp->DoesSocketExist(FName("root")))
		{
			const FRotator RootRot = MeshComp->GetSocketRotation(FName("root"));
			OutForward = UKismetMathLibrary::GetRightVector(RootRot); 
			OutLocation += RootRot.RotateVector(DetectionOffset); 
		}
		else
		{
			OutForward = MeshComp->GetRightVector();
			OutLocation += MeshComp->GetComponentRotation().RotateVector(DetectionOffset);
		}
	}
}

void UAnimNotifyState_TargetSectorDetection::DrawDebug(USkeletalMeshComponent* MeshComp)
{
	const UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!IsValid(World)) return;

	FVector Loc;
	FVector Fwd;
	GetSearchInfo(MeshComp, Loc, Fwd);

	const bool bIsGameWorld = World->IsGameWorld();
	const FVector Up = (bIsGameWorld && MeshComp->GetOwner()) ? MeshComp->GetOwner()->GetActorUpVector() : MeshComp->GetUpVector();
	
	const float HalfH = FMath::Max(Height * 0.5f, 0.f);
	const float AngleRad = FMath::DegreesToRadians(Angle);
	const int32 NumSeg = 24;
	const float LifeTime = bIsGameWorld ? 0.1f : 0.f;

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
		TopArc.Add(Loc + Dir * Radius + Up * HalfH);
		BottomArc.Add(Loc + Dir * Radius - Up * HalfH);
	}

	FColor DrawColor = AffectedActors.Num() > 0 ? FColor::Green : FColor::Red;

	for (int32 i = 0; i < NumSeg; ++i)
	{
		DrawDebugLine(World, TopArc[i], TopArc[i + 1], DrawColor, false, LifeTime);
		DrawDebugLine(World, BottomArc[i], BottomArc[i + 1], DrawColor, false, LifeTime);
	}

	if (TopArc.Num() > 0)
	{
		DrawDebugLine(World, BottomArc[0], TopArc[0], DrawColor, false, LifeTime);
		DrawDebugLine(World, BottomArc.Last(), TopArc.Last(), DrawColor, false, LifeTime);
		DrawDebugLine(World, Loc - Up * HalfH, BottomArc[0], DrawColor, false, LifeTime);
		DrawDebugLine(World, Loc + Up * HalfH, TopArc[0], DrawColor, false, LifeTime);
		DrawDebugLine(World, Loc - Up * HalfH, BottomArc.Last(), DrawColor, false, LifeTime);
		DrawDebugLine(World, Loc + Up * HalfH, TopArc.Last(), DrawColor, false, LifeTime);
	}
}
