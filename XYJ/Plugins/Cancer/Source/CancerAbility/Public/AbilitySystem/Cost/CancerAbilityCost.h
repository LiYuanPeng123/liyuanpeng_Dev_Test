
#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "CancerAbilityCost.generated.h"

struct FGameplayAbilityActorInfo;
class UCancerGameplayAbility;
/**
 * 消耗
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class CANCERABILITY_API UCancerAbilityCost : public UObject
{
	GENERATED_BODY()
public:
	UCancerAbilityCost(){}
	
	virtual bool CheckCost(const UCancerGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
	{
		return true;
	}

	virtual void ApplyCost(const UCancerGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
	{
	}

	bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }

protected:
	/** If true, this cost should only be applied if this ability hits successfully */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs,DisplayName="命中时应用消耗")
	bool bOnlyApplyCostOnHit = false;
};
