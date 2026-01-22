
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "CombatAbility_DamageFeedback.generated.h"

/**
 * 造成伤害后 根据造成的伤害 造成反馈
 * 例如被弹反、吸血类处理、连锁伤害等
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_DamageFeedback : public UCancerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UCombatAbility_DamageFeedback(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	// -- Begin Gameplay Ability implementation
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
								 const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;
	
	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;
	
};
