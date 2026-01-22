#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "UObject/Object.h"
#include "CombatAbility_Move.generated.h"

struct FInputActionValue;
/**
 * 移动
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_Move : public UCancerGameplayAbility
{
	GENERATED_BODY()

public:
	UCombatAbility_Move(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
	                        const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo,
	                        bool bReplicateEndAbility, bool bWasCancelled) override;


	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;

	virtual void AbilityInputTriggered_Implementation() override;

	virtual void AbilityInputReleased_Implementation() override;
	

	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler|Move")
	void Move(const FInputActionValue& Value) const;

private:
};
