 #pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CombatAbility_Weakness.generated.h"

class UAbilityTask_WaitDelay;
/**
 * 虚弱
 */
 UCLASS()
 class CANCERABILITY_API UCombatAbility_Weakness : public UCancerPlayAnimationAbility
 {
	 GENERATED_BODY()

 public:
	 UCombatAbility_Weakness();

	 virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                              const FGameplayAbilityActivationInfo ActivationInfo,
	                              const FGameplayEventData* TriggerEventData) override;

	 virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                         const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                         bool bWasCancelled) override;

	 virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;


	 UFUNCTION()
	 void HandleWeakFinish();

 protected:
	 UPROPERTY()
	 UAbilityTask_WaitDelay* WaitDelay;
 };
