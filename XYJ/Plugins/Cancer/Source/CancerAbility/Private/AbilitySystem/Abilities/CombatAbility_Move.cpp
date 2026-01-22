#include "AbilitySystem/Abilities/CombatAbility_Move.h"

#include "CancerLockingFunctionLibrary.h"
#include "CancerNativeGamePlayTag.h"
#include "InputActionValue.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "CancerInput/Public/CancerInputValuePayload.h"
#include "Kismet/KismetMathLibrary.h"

class UCancerInputValuePayload;

UCombatAbility_Move::UCombatAbility_Move(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCombatAbility_Move::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}


void UCombatAbility_Move::Move_Implementation(const FInputActionValue& Value) const
{
	FVector2D InputValue = Value.Get<FVector2D>();
	if (auto OwnerPawn = Cast<APawn>(GetOwningActorFromActorInfo()))
	{
		FRotator ControlRot = OwnerPawn->GetControlRotation();
		const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
		FVector ForwardDir = FRotationMatrix(YawRot).GetScaledAxis(EAxis::X);
		FVector RightDir = FRotationMatrix(YawRot).GetScaledAxis(EAxis::Y);
		if (auto LockTargetComponent = UCancerLockingFunctionLibrary::GetLockingTargetComponent(OwnerPawn))
		{
			if (LockTargetComponent->IsLockTarget() && LockTargetComponent->CurLockingPointComponent)
			{
				if (AActor* LockTargetActor = LockTargetComponent->CurLockingPointComponent->GetOwner())
				{
					FVector FlatToTarget = LockTargetActor->GetActorLocation() - OwnerPawn->GetActorLocation();
					FlatToTarget.Z = 0.f;

					if (FlatToTarget.Normalize())
					{
						ForwardDir = FlatToTarget;

						RightDir = FVector::CrossProduct(FVector::UpVector, ForwardDir);
						if (!RightDir.Normalize())
						{
							RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
						}
					}
					else
					{
						ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
						RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
					}
				}
			}
		}
		OwnerPawn->AddMovementInput(RightDir, InputValue.X);
		OwnerPawn->AddMovementInput(ForwardDir, InputValue.Y);
	}
}

void UCombatAbility_Move::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCombatAbility_Move::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);

	if (!CanReceivedInput(Payload)) return;

	const UCancerInputValuePayload* InputValuePayload = Cast<UCancerInputValuePayload>(Payload.OptionalObject2);

	if (Payload.EventTag == Tag_Combat_Event_Input_Trigger)
	{
		Move(InputValuePayload->InputValue);
	}
	else if (Payload.EventTag == Tag_Combat_Event_Input_Completed)
	{
		K2_EndAbility();
	}
}

void UCombatAbility_Move::AbilityInputTriggered_Implementation()
{
	Super::AbilityInputTriggered_Implementation();
}

void UCombatAbility_Move::AbilityInputReleased_Implementation()
{
	Super::AbilityInputReleased_Implementation();
	K2_EndAbility();
}
