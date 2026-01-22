#include "AbilitySystem/Abilities/CombatAbility_Look.h"

#include "CancerInputValuePayload.h"
#include "CancerNativeGamePlayTag.h"
#include "InputActionValue.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

UCombatAbility_Look::UCombatAbility_Look(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCombatAbility_Look::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}

void UCombatAbility_Look::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCombatAbility_Look::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);

	if (!CanReceivedInput(Payload)) return;

	const UCancerInputValuePayload* InputValuePayload = Cast<UCancerInputValuePayload>(Payload.OptionalObject2);

	if (Payload.EventTag == Tag_Combat_Event_Input_Trigger)
	{
		Look(InputValuePayload->InputValue);
	}
	else if (Payload.EventTag == Tag_Combat_Event_Input_Completed)
	{
		K2_EndAbility();
	}
	bIsTriggered = false;
}

void UCombatAbility_Look::AbilityInputReleased_Implementation()
{
	Super::AbilityInputReleased_Implementation();
	K2_EndAbility();
}

void UCombatAbility_Look::AbilityInputTriggered_Implementation()
{
	Super::AbilityInputTriggered_Implementation();
}

void UCombatAbility_Look::Look_Implementation(const FInputActionValue& Value) const
{
	FVector2D InputValue = Value.Get<FVector2D>();
	APawn* Pawn = Cast<APawn>(GetOwningActorFromActorInfo());
	if (IsValid(Pawn))
	{
		Pawn->AddControllerYawInput(InputValue[0]);
		Pawn->AddControllerPitchInput(InputValue[1]);
	}
}
