#include "AbilitySystem/Abilities/CombatAbility_Death.h"
#include "CancerAbilityFunctionLibrary.h"

UCombatAbility_Death::UCombatAbility_Death()
{
	// Old Way
	// FAbilityTriggerData TriggerData;
	// TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	// TriggerData.TriggerTag = Tag_Combat_Event_Death;
	// AbilityTriggers.Add(TriggerData);
}

void UCombatAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCombatAbility_Death::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);
	
}
