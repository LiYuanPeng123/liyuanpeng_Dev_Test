#include "AbilitySystem/Abilities/CombatAbility_Evade.h"
#include "AbilitySystemComponent.h"
#include "CancerAbilitySystemComponent.h"
#include "CancerNativeGamePlayTag.h"
#include "Components/CancerHeroComponent.h"
#include "Components/CancerMotionWarpingComponent.h"

UCombatAbility_Evade::UCombatAbility_Evade(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCombatAbility_Evade::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCombatAbility_Evade::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	ExecuteIndex = 0;
	StartAccel2D = FVector::ZeroVector;
}


void UCombatAbility_Evade::OnAnimationReady_Implementation()
{
	Super::OnAnimationReady_Implementation();
}

void UCombatAbility_Evade::OnAnimationFinished_Implementation()
{
	Super::OnAnimationFinished_Implementation();

	ActorYaw = GetOwningActorFromActorInfo()->GetActorRotation().Yaw;

	auto MotionComponent = GetMotionWarpingComponentFromActorInfo();
	if (!MotionComponent) return;
	MotionComponent->RemoveWarpTarget(WarpName);
	MotionComponent->RemoveWarpTarget(GroundWarpName);
}

void UCombatAbility_Evade::OnAnimationStarted_Implementation()
{
	Super::OnAnimationStarted_Implementation();
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	auto MotionComponent = GetMotionWarpingComponentFromActorInfo();
	if (!MotionComponent || !ASC) return;

	MotionComponent->RemoveWarpTarget(WarpName);
	MotionComponent->RemoveWarpTarget(GroundWarpName);

	//锁定
	FRotator LockTargetRotation = FRotator();
	bool bIsLock = false;
	if (auto Component = GetHeroComponentFromActorInfo())
	{
		if (auto Target = Component->CombatTarget)
		{
			LockTargetRotation = UKismetMathLibrary::FindLookAtRotation(
				GetOwningActorFromActorInfo()->GetActorLocation(),
				Target->GetActorLocation());
			bIsLock = true;
		}
	}
	//空中
	if (ASC->HasTag(Movement_Air_Falling) || ASC->HasTag(Movement_Air_Flying))
	{
		auto PC = GetController<AController>();
		float CameraPitch = FRotator::NormalizeAxis(PC->GetControlRotation().Pitch);
		bool HasInput = !StartAccel2D.IsNearlyZero();
		float InputYaw = StartAccel2D.IsNearlyZero() ? ActorYaw : StartAccel2D.Rotation().Yaw;
		bool bIsBackward = StartAccel2D.IsNearlyZero();
		float AppliedPitch = bIsBackward ? -CameraPitch : CameraPitch;
		AppliedPitch = FMath::Clamp(AppliedPitch, -AerialPitchOffset.X, AerialPitchOffset.Y);
		FVector CurrentLoc = GetOwningActorFromActorInfo()->GetActorLocation();

		if (bOrientToMovement)
		{
			float DisplacementYaw = bIsBackward ? FRotator::NormalizeAxis(ActorYaw + 180.f) : InputYaw;
			FVector TargetLoc = CurrentLoc + FRotator(AppliedPitch, DisplacementYaw, 0.f).Vector() *
				DefaultEvadeDistance;
			if (!bIsBackward)
			{
				FRotator TargetRot = GetOwningActorFromActorInfo()->GetActorRotation();
				TargetRot.Yaw = InputYaw;
				TargetRot.Pitch = 0.f;
				TargetRot.Roll = 0.f;

				// GroundWarpName 仅在有输入时更新转向
				MotionComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
					GroundWarpName, FVector::ZeroVector, TargetRot);
			}
			MotionComponent->AddOrUpdateWarpTargetFromLocation(WarpName, TargetLoc);
		}
		else
		{
			//没有输入获取角色朝向目标方向
			FRotator LookRotation = FRotator();
			if (!HasInput)
			{
				if (auto Component = GetHeroComponentFromActorInfo())
				{
					if (auto Target = Component->CombatTarget)
					{
						LookRotation = UKismetMathLibrary::FindLookAtRotation(
							GetOwningActorFromActorInfo()->GetActorLocation(),
							Target->GetActorLocation());
					}
				}
			}
			float DisplacementYaw = bIsBackward ? FRotator::NormalizeAxis(LookRotation.Yaw + 180) : InputYaw;
			FVector TargetLoc = CurrentLoc + FRotator(AppliedPitch, DisplacementYaw, 0.f).Vector() *
				DefaultEvadeDistance;
			//DrawDebugSphere(GetWorld(), TargetLoc, 30, 10, FColor::Red, false, 3, 1, 2);
			MotionComponent->AddOrUpdateWarpTargetFromLocation(WarpName, TargetLoc);
		}
	}
	else
	{
		FVector CurrentLoc = GetOwningActorFromActorInfo()->GetActorLocation();
		//没有输入 向后位移  1.锁定方向为看向敌人方向 +180 2.为锁定使用动画位移
		if (StartAccel2D.IsNearlyZero())
		{
			if (bIsLock)
			{
				float DisplacementYaw = FRotator::NormalizeAxis(LockTargetRotation.Yaw + 180.f);
				FVector TargetLoc = CurrentLoc + FRotator(0, DisplacementYaw, 0.f).Vector() *
					DefaultEvadeDistance;
				MotionComponent->AddOrUpdateWarpTargetFromLocation(WarpName, TargetLoc);
			}
		}
		else
		{
			auto Rotation = GetOwningActorFromActorInfo()->GetActorRotation();
			Rotation.Yaw = StartAccel2D.Rotation().Yaw;
			FVector TargetLoc = CurrentLoc + FRotator(0, Rotation.Yaw, 0.f).Vector() *
				DefaultEvadeDistance;
			MotionComponent->AddOrUpdateWarpTargetFromLocation(WarpName, TargetLoc);
			MotionComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
				GroundWarpName, FVector::ZeroVector, Rotation);
		}
	}
}

void UCombatAbility_Evade::AbilityInputStarted_Implementation()
{
	Super::AbilityInputStarted_Implementation();
	if (auto Movement = GetCancerMovementFromActorInfo())
	{
		StartAccel2D = Movement->GetCurrentAcceleration().GetSafeNormal2D();
		bOrientToMovement = Movement->GetCurrentRotationMode();
	}
	FName OrientSection = StartAccel2D.IsNearlyZero() ? OrientDefaultSection : FName("F");

	CurSection = bOrientToMovement ? OrientSection : GetLockTargetSectionName(StartAccel2D);

	UE_LOG(LogTemp, Warning, TEXT("闪避%s"), *CurSection.ToString());
	if (InputIndex == 1)
	{
		SetupAndPlayAnimation(GetMontage(), CurSection);
		ActorYaw = GetOwningActorFromActorInfo()->GetActorRotation().Yaw;
		ExecuteIndex++;
		return;
	}
	if (HasMatchTag(Tag_Combat_State_PostWindow) && ExecuteIndex < MaxExecuteIndex)
	{
		SetupAndPlayAnimation(GetMontage(), CurSection);
		ExecuteIndex++;
	}
}
