
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CombatAbility_Block.generated.h"

class UAbilityTask_WaitInputRelease;
/**
 *  格挡技能
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_Block : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()
public:
	UCombatAbility_Block(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;


	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;

	UFUNCTION()
	void HandleInputReleased(float TimeHeld);
	
};
