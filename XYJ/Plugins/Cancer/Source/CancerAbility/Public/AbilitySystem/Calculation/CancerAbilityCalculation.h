#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "Types/FCombatAbilitySet.h"
#include "CancerAbilityCalculation.generated.h"

class UAbilityDef;
/**
 * 
 */
UCLASS(Abstract)
class CANCERABILITY_API UCancerAbilityCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Cancer|Calculation",
		meta = (DisplayName = "Get Captured Attribute Magnitude User", ScriptName = "GetCapturedAttributeMagnitude"))
	float BP_GetCapturedAttributeMagnitude(const FGameplayEffectSpec& EffectSpec, FGameplayAttribute Attribute) const;

	UFUNCTION(BlueprintPure, Category = "Cancer|Calculation",
		Meta = (Categories = "SetByCaller", DisplayName = "GetFloatMagnitudeByTag"))
	float BP_GetFloatMagnitudeByTag(const FGameplayEffectSpec& EffectSpec, const FGameplayTag Tag) const;

	UFUNCTION(BlueprintPure, Category = "Cancer|Calculation", Meta = ( DisplayName = "HasTag"))
	bool BP_HasTag(const FGameplayEffectSpec& EffectSpec, const FGameplayTag Tag) const;

	UFUNCTION(BlueprintPure, Category = "Cancer|Calculation", Meta = (DisplayName = "GetLevel"))
	float BP_GetLevel(const FGameplayEffectSpec& EffectSpec) const;

	UFUNCTION(BlueprintPure, Category = "Cancer|Calculation", Meta = (DisplayName = "GetDefinition"))
	FCancerAbilityDef BP_GetDefinition(const FGameplayEffectSpec& EffectSpec) const;

	UFUNCTION(BlueprintPure, Category = "Cancer|Calculation", Meta = (DisplayName = "GetCoolDown"))
	float BP_GetCoolDown(const FGameplayEffectSpec& EffectSpec) const;

	UFUNCTION(BlueprintPure, Category = "Cancer|Calculation", Meta = (DisplayName = "GetCost"))
	float BP_GetCost(const FGameplayEffectSpec& EffectSpec) const;

	UFUNCTION(BlueprintPure, Category = "Cancer|Calculation", Meta = (DisplayName = "GetOwner"))
	AActor* BP_GetInstigator(const FGameplayEffectSpec& EffectSpec) const;

	UFUNCTION(BlueprintPure, Category = "Cancer|Calculation", Meta = (DisplayName = "GetAttributeSet"))
	float BP_GetAttributeSet(const FGameplayEffectSpec& EffectSpec,const FGameplayTag& InTag) const;
};
