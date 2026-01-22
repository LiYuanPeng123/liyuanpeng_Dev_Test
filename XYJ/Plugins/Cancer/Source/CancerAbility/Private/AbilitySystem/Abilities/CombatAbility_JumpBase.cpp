#include "AbilitySystem/Abilities/CombatAbility_JumpBase.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCombatAbility_JumpBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCombatAbility_JumpBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCombatAbility_JumpBase::ApplyJumpMovementOverrides(const AActor* ActorInfo)
{
	const ACharacter* Character = Cast<ACharacter>(ActorInfo);
	if (!IsValid(Character)) return;

	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		if (bOverrideJumpZVelocity)
		{
			PrevJumpZVelocity = Move->JumpZVelocity;
			Move->JumpZVelocity = JumpZVelocityOverride;
		}
		if (bOverrideGravityScale)
		{
			PrevGravityScale = Move->GravityScale;
			Move->GravityScale = GravityScaleOverride;
		}
		if (bOverrideAirRotationRateYaw)
		{
			PrevRotationRate = Move->RotationRate;
			FRotator NewRate = PrevRotationRate;
			NewRate.Yaw = AirRotationRateYaw;
			Move->RotationRate = NewRate;
		}
		if (bOverrideMaxAcceleration)
		{
			PrevMaxAcceleration = Move->MaxAcceleration;
			const bool bGrounded = Move->IsMovingOnGround();
			if (bGrounded)
			{
				Move->MaxAcceleration = MaxAccelerationOverride;
			}
		}
	}
}

void UCombatAbility_JumpBase::RestoreJumpMovementOverrides(const AActor* ActorInfo)
{
	const ACharacter* Character = Cast<ACharacter>(ActorInfo);
	if (!IsValid(Character)) return;

	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		if (bOverrideJumpZVelocity)
		{
			Move->JumpZVelocity = PrevJumpZVelocity;
		}
		if (bOverrideGravityScale)
		{
			Move->GravityScale = PrevGravityScale;
		}
		if (bOverrideAirRotationRateYaw)
		{
			Move->RotationRate = PrevRotationRate;
		}
		if (bOverrideMaxAcceleration)
		{
			Move->MaxAcceleration = PrevMaxAcceleration;
		}
	}
}
