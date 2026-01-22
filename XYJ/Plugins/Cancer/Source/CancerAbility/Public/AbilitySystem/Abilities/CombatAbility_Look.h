// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "CombatAbility_Look.generated.h"

class UAbilityTask_WaitInputRelease;
struct FInputActionValue;
/**
 * 查看
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_Look : public UCancerGameplayAbility
{
	GENERATED_BODY()
public:

	UCombatAbility_Look(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;


	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;
	

	virtual void AbilityInputReleased_Implementation() override;
	virtual void AbilityInputTriggered_Implementation() override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler|Move")
	void Look( const FInputActionValue& Value) const;
};
