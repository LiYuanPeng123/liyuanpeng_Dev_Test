
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CombatAbility_LoopAbility.generated.h"

class UAbilityTask_WaitInputRelease;

/**
 * 蓄力技能
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_LoopAbility : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()
	public:

	UCombatAbility_LoopAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


	UFUNCTION()
	void HandleInputReleased(float TimeHeld);

	UPROPERTY(BlueprintReadOnly)
	bool bInputRelease = true;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,DisplayName="蒙太奇标签")
	FGameplayTag MontageTag;

protected:
	UPROPERTY(BlueprintReadOnly)
	UAbilityTask_WaitInputRelease*WaitReleaseTask;
};
