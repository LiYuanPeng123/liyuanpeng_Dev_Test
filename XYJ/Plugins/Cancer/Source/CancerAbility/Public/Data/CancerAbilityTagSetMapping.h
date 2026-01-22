#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/FCombatAbilitySet.h"
#include "CancerAbilityTagSetMapping.generated.h"

UCLASS()
class CANCERABILITY_API UCancerAbilityTagSetMapping : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<FCombatAbilitySet> AbilityPrioritySet;

	int32 GetTagPriority(const FGameplayTag& Tag);

	ECancerAbilityGroup GetTagGroup(const FGameplayTag& Tag);

	FCombatAbilitySet GetAbilitySet(const FGameplayTag& Tag);

	bool GetAbilityByBlock(const FGameplayTag& Ability, const FGameplayTagContainer& ActivateAbility);
	FGameplayTagContainer GetAbilityTagByPriority(const FGameplayTagContainer& Ability);*/
};
