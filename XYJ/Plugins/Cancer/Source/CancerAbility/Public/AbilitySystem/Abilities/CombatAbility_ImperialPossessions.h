#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "AbilityTasks/AbilityTask_PerformTargeting.h"
#include "CombatAbility_ImperialPossessions.generated.h"

class UAbilityTask_TargetingTick;
class UTargetingPreset;
/**
 * 御物
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_ImperialPossessions : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()

public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
									const FGameplayTagContainer* SourceTags = nullptr,
									const FGameplayTagContainer* TargetTags = nullptr,
									FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;

	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability")
	TObjectPtr<UTargetingPreset> TargetingPreset;
	
	UAbilityTask_TargetingTick* InitializeTargetingPresetTask(UTargetingPreset* InTargetingPreset);
	
protected:
	UFUNCTION()
	virtual void HandleTargetReady(const FTargetingRequestHandle Payload);

	UPROPERTY()
	TObjectPtr<UAbilityTask_TargetingTick> TargetingTick;
};
