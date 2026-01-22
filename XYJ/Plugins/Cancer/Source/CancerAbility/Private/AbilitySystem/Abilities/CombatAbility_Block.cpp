#include "AbilitySystem/Abilities/CombatAbility_Block.h"
#include "CancerAbilitySystemComponent.h"
#include "CancerInputValuePayload.h"
#include "CancerNativeGamePlayTag.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Types/FCancerCombatAnimNotifyHelpers.h"

UCombatAbility_Block::UCombatAbility_Block(const FObjectInitializer& ObjectInitializer)
{
}

void UCombatAbility_Block::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (IsAIControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("UCombatAbility_Block  AI Block "))
	}
	
}

void UCombatAbility_Block::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UE_LOG(LogTemp, Warning, TEXT("UCombatAbility_Block End block"));
}

void UCombatAbility_Block::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);
}

void UCombatAbility_Block::HandleInputReleased(float TimeHeld)
{
	K2_EndAbility();
}
