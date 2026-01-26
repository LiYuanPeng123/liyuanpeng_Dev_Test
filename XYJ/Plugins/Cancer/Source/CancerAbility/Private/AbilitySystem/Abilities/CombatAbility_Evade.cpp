#include "AbilitySystem/Abilities/CombatAbility_Evade.h"
#include "AbilitySystemComponent.h"
#include "CancerAbilitySystemComponent.h"
#include "CancerNativeGamePlayTag.h"
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
}

void UCombatAbility_Evade::OnAnimationStarted_Implementation()
{
	Super::OnAnimationStarted_Implementation();
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	auto MotionComponent = GetMotionWarpingComponentFromActorInfo();
	if (!MotionComponent || !ASC) return;
	
	MotionComponent->RemoveWarpTarget(WarpName);

	if (ASC->HasTag(Movement_Falling) || ASC->HasTag(Movement_Flying))
	{
		//空中
		if (auto PC = GetController<APlayerController>())
		{
			float CameraPitch = FRotator::NormalizeAxis(PC->GetControlRotation().Pitch);

			float ControllerYaw = PC->GetControlRotation().Yaw;
			float InputYaw = StartAccel2D.Rotation().Yaw;
			float DeltaYaw = FMath::Abs(FMath::FindDeltaAngleDegrees(ControllerYaw, InputYaw));
			bool bIsBackward = DeltaYaw > 90.0f;
			if (!StartAccel2D.IsNearlyZero())
			{
				bIsBackward = true;
			}

			float AppliedPitch = bIsBackward ? -CameraPitch : CameraPitch;
			AppliedPitch = FMath::Clamp(AppliedPitch, -MaxAerialPitchOffset, MaxAerialPitchOffset);

			FRotator TargetRot(AppliedPitch, InputYaw, 0.f);
			MotionComponent->AddOrUpdateWarpTargetFromLocationAndRotation(WarpName, FVector::ZeroVector, TargetRot);
		}
	}
	else
	{
		//地面
		auto Rotation = GetOwningActorFromActorInfo()->GetActorRotation();
		if (!StartAccel2D.IsNearlyZero())
		{
			Rotation.Yaw = StartAccel2D.Rotation().Yaw;
			MotionComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
				WarpName, FVector::ZeroVector, Rotation);
		}
	}
}

void UCombatAbility_Evade::AbilityInputStarted_Implementation()
{
	Super::AbilityInputStarted_Implementation();
	bool bOrientToMovement = true;
	if (auto Movement = GetCancerMovementFromActorInfo())
	{
		StartAccel2D = Movement->GetCurrentAcceleration().GetSafeNormal2D();
		bOrientToMovement = Movement->GetCurrentRotationMode();
	}
	FName Section = bOrientToMovement ? OrientDefaultSection : GetLockTargetSectionName(StartAccel2D);
	if (InputIndex == 1)
	{
		SetupAndPlayAnimation(GetMontage(), Section);
		ExecuteIndex++;
		return;
	}
	if (HasMatchTag(Tag_Combat_State_PostWindow) && ExecuteIndex < MaxExecuteIndex)
	{
		SetupAndPlayAnimation(GetMontage(), Section);
		ExecuteIndex++;
	}
}
