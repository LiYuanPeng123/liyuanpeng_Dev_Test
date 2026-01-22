#pragma once
#include "CoreMinimal.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CombatAbility_Death.generated.h"

/**
 * 死亡
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_Death : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()

public:
	UCombatAbility_Death();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;
	
};
