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
		// 切换到 Flying 模式以避免 Z 轴位移受重力影响
		if (bForceFlyingMode)
		{
			if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
			{
				if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
				{
					MoveComp->SetMovementMode(MOVE_Flying);
				}
			}
		}
	}

	UpdateWarpTarget(MeshComp);
}

void UAnimNotifyState_DisplacementWarp::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	// 结束时让移动组件自己去检测并恢复正确的移动模式
	if (bForceFlyingMode)
	{
		if (AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr)
		{
			if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
			{
				if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
				{
					MoveComp->SetDefaultMovementMode();
				}
			}
		}
	}
}

void UAnimNotifyState_DisplacementWarp::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                   float FrameDeltaTime,
                                                   const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	Debug(MeshComp);

	if (bDynamicUpdateTarget)
	{
		if (AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr)
		{
			bool bShouldUpdate = true;
			if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
			{
				float CurrentTime = 0.f;
				if (const UAnimMontage* Montage = Cast<UAnimMontage>(Animation))
				{
					CurrentTime = AnimInstance->Montage_GetPosition(Montage);
				}
				else
				{
					CurrentTime = MeshComp->GetPosition();
				}
				
				if (UMotionWarpingComponent* MotionWarpingComp = OwnerActor->FindComponentByClass<UMotionWarpingComponent>())
				{
					// 尝试找到对应的 Modifier
					for (const auto& Modifier : MotionWarpingComp->GetModifiers())
					{
						if (Modifier && Modifier->Animation == Animation && Modifier->StartTime <= CurrentTime && Modifier->EndTime >= CurrentTime)
						{
							// 找到了当前激活的 Modifier，计算当前进度的 Alpha (0-1)
							float ModifierDuration = Modifier->EndTime - Modifier->StartTime;
							if (ModifierDuration > KINDA_SMALL_NUMBER)
							{
								float Alpha = (CurrentTime - Modifier->StartTime) / ModifierDuration;
								// 检查是否在允许的动态修正时间范围内
								if (Alpha < DynamicCorrectionRange.X || Alpha > DynamicCorrectionRange.Y)
								{
									bShouldUpdate = false;
								}
							}
							break;
						}
					}
				}
			}

			if (bShouldUpdate)
			{
				UpdateWarpTarget(MeshComp);
			}
		}
	}
}

void UAnimNotifyState_DisplacementWarp::GetSearchInfo(USkeletalMeshComponent* MeshComp, FVector& OutLocation,
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
		// 应用偏移
		OutLocation += OwnerRot.RotateVector(AutoAdsorptionOffset);
	}
	else
	{
		// 编辑器预览或无 Owner
		OutLocation = MeshComp->GetComponentLocation();
		// 使用 Socket 旋转作为参考可能更准确，如果没有 root socket 则退化为组件旋转
		if (MeshComp->DoesSocketExist(FName("root")))
		{
			const FRotator RootRot = MeshComp->GetSocketRotation(FName("root"));
			// 修正方向：在预览中 Root 骨骼的前向通常对应角色的右向（取决于骨骼绑定习惯），
			// 这里假设预览时 RightVector 是前向，或者直接用 Component 的前向。
			// 为了与 Debug 中的 UKismetMathLibrary::GetRightVector(Rotation) 保持一致：
			OutForward = UKismetMathLibrary::GetRightVector(RootRot); 
			
			// 偏移需要基于当前的旋转
			// 注意：预览时坐标系可能不同，通常 Component 的 Transform 就是模拟的 Actor Transform
			FVector Offset = AutoAdsorptionOffset;
			// 如果预览方向有问题，这里可能需要额外旋转 Offset，暂时按世界坐标系旋转处理
			OutLocation += RootRot.RotateVector(Offset); 
		}
		else
		{
			OutForward = MeshComp->GetRightVector(); // 假设 Mesh 的 X 轴朝前
			OutLocation += MeshComp->GetComponentRotation().RotateVector(AutoAdsorptionOffset);
		}
	}
}

void UAnimNotifyState_DisplacementWarp::UpdateWarpTarget(USkeletalMeshComponent* MeshComp) const
{
	AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!OwnerActor) return;

	UMotionWarpingComponent* MotionWarpingComp = OwnerActor->FindComponentByClass<UMotionWarpingComponent>();
	UCancerMotionWarpingComponent* CancerComp = Cast<UCancerMotionWarpingComponent>(MotionWarpingComp);
	if (!CancerComp) return;

	bool DesiredSprint = false;
	if (auto Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent(OwnerActor))
	{
		DesiredSprint = Movement->IsDesiredSprint();
	}

	if (auto HeroComponent = UCancerHeroComponent::FindHeroComponent(OwnerActor))
	{
		if (!DesiredSprint)
		{
			const AActor* Target = HeroComponent->GetCombatTarget();
			// 即使 Target 为空也调用 UpdateLocationAndRotation_Character，
			// 因为组件内部现在会处理目标丢失的情况（使用最后一次有效位置）
			const float MaxClampDistance = MaxDisplacementWithTarget;
			CancerComp->UpdateLocationAndRotation_Character(
				Target, MaxClampDistance, FinalOffset, WarpTargetName, /*bCancelWarpIfCloser*/ true, HeightAlignment, CustomHeightOffset);
			return;
		}
	}

	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (bAutoAdsorption && OwnerPawn && OwnerPawn->GetController() && OwnerPawn->GetController()->IsA(APlayerController::StaticClass()))
	{
		const UWorld* World = OwnerActor->GetWorld();
		if (IsValid(World))
		{
			FVector SearchOrigin;
			FVector Forward;
			GetSearchInfo(MeshComp, SearchOrigin, Forward);

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
				SearchOrigin,
				FQuat::Identity,
				ObjParams,
				FCollisionShape::MakeSphere(FMath::Max(Radius, 0.f)),
				TraceParams
			);

			AActor* BestTarget = nullptr;
			float BestDistSq2D = TNumericLimits<float>::Max();
			if (bHasOverlap)
			{
				for (const FOverlapResult& Overlap : Overlaps)
				{
					AActor* HitActor = Overlap.GetActor();
					if (!IsValid(HitActor) || HitActor == OwnerActor)
					{
						continue;
					}
					const FVector TargetLoc = HitActor->GetActorLocation();
					const FVector ToTarget = TargetLoc - SearchOrigin;
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
					if (!UCancerLockingFunctionLibrary::IsCanLock(OwnerActor, HitActor))
					{
						continue;
					}
					const float DistSq2D = FVector::DistSquared2D(SearchOrigin, TargetLoc);
					if (DistSq2D < BestDistSq2D)
					{
						BestDistSq2D = DistSq2D;
						BestTarget = HitActor;
					}
				}
				if (BestTarget)
				{
					const float MaxClampDistance = MaxDisplacementWithTarget;
					CancerComp->UpdateLocationAndRotation_Character(
						BestTarget, MaxClampDistance, FinalOffset, WarpTargetName, /*bCancelWarpIfCloser*/ true, HeightAlignment, CustomHeightOffset);
					return;
				}
			}
		}
	}
}


void UAnimNotifyState_DisplacementWarp::Debug(USkeletalMeshComponent* MeshComp)
{
	if (!MeshComp) return;

	AActor* OwnerActor = MeshComp->GetOwner();
	const UWorld* World = MeshComp->GetWorld();
	if (!IsValid(World)) return;

	const bool bIsGameWorld = World->IsGameWorld();
	const bool bShouldDraw = bIsGameWorld ? AdsorptionVar.GetValueOnGameThread() : bDebug;
	if (!bShouldDraw) return;

	FVector Loc;
	FVector Fwd;
	GetSearchInfo(MeshComp, Loc, Fwd);

	const FVector Up = bIsGameWorld && OwnerActor ? OwnerActor->GetActorUpVector() : MeshComp->GetUpVector();
	const float HalfH = FMath::Max(Height * 0.5f, 0.f);
	const float AngleRad = FMath::DegreesToRadians(Angle);
	const int32 NumSeg = 24;
	
	// 在编辑器预览中使用固定时长，游戏中使用设置的时长
	const float LifeTime = bIsGameWorld ? UCancerAbilitySetting::Get()->MeleeScanDebugDuration : 0.f;

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
