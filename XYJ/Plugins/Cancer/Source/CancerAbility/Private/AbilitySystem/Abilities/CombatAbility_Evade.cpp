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
	auto MotionComponent = GetMotionWarpingComponentFromActorInfo();
	if (!MotionComponent ) return;
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
	
	if (ASC->HasTag(Movement_Falling) || ASC->HasTag(Movement_Flying))
	{
		//空中
		if (auto PC = GetController<APlayerController>())
		{
			// 获取摄像机 Pitch (Normalized: -180 to 180, negative is up, positive is down)
			float CameraPitch = FRotator::NormalizeAxis(PC->GetControlRotation().Pitch);

			float ControllerYaw = PC->GetControlRotation().Yaw;
			float InputYaw = StartAccel2D.IsNearlyZero() ? ControllerYaw : StartAccel2D.Rotation().Yaw;
			
			bool bOrientToMovement = true;
			if (auto Movement = GetCancerMovementFromActorInfo())
			{
				StartAccel2D = Movement->GetCurrentAcceleration().GetSafeNormal2D();
				bOrientToMovement = Movement->GetCurrentRotationMode();
			}
			FName OrientSection = StartAccel2D.IsNearlyZero() ? OrientDefaultSection : FName("F");
			FName Section = bOrientToMovement ? OrientSection : GetLockTargetSectionName(StartAccel2D);
			
			bool bIsBackward =  StartAccel2D.IsNearlyZero();
			if (bOrientToMovement)
			{
				float AppliedPitch = bIsBackward ? -CameraPitch : CameraPitch;
				AppliedPitch = FMath::Clamp(AppliedPitch, -AerialPitchOffset.X, AerialPitchOffset.Y);

				FVector CurrentLoc = GetOwningActorFromActorInfo()->GetActorLocation();

				FVector TargetLoc = CurrentLoc + FRotator(AppliedPitch, InputYaw, 0.f).Vector() * DefaultEvadeDistance;


				FRotator TargetRot = GetOwningActorFromActorInfo()->GetActorRotation();
				if (!StartAccel2D.IsNearlyZero())
				{
					TargetRot.Yaw = InputYaw;
				}
				TargetRot.Pitch = 0.f;
				TargetRot.Roll = 0.f;
				DrawDebugSphere(GetWorld(), TargetLoc, 30, 10, FColor::Red, false, 3, 1, 2);
				MotionComponent->AddOrUpdateWarpTargetFromLocation(WarpName, TargetLoc);
				MotionComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
				GroundWarpName, FVector::ZeroVector, TargetRot);
			}
			else
			{
				
			}
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
				GroundWarpName, FVector::ZeroVector, Rotation);
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
	FName OrientSection = StartAccel2D.IsNearlyZero() ? OrientDefaultSection : FName("F");
	FName Section = bOrientToMovement ? OrientSection : GetLockTargetSectionName(StartAccel2D);
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
