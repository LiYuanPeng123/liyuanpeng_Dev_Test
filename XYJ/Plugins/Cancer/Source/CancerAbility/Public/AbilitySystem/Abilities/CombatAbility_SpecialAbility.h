#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "Data/CancerCombatData.h"
#include "CombatAbility_SpecialAbility.generated.h"

/**
 * 
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_SpecialAbility : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;

	FWeaponData* GetCurComboData(FGameplayTag ComboTag);

	UFUNCTION()
	void HandleInputReleased(float TimeHeld);

protected:
	UPROPERTY()
	class UAbilityTask_WaitInputRelease* WaitReleaseTask;

	FWeaponData* CurrentWeaponData;
	float StartChargeTime;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TMap<FGameplayTag, FWeaponData> WeaponDatas;
};
