#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDefinition.h"
#include "InventoryFragment_AbilitySet.generated.h"

class UCancerAbilitySet;
/**
 * 
 */
UCLASS(DisplayName="赋予能力")
class CANCERINVENTORY_API UInventoryFragment_AbilitySet : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category=Equipment, DisplayName="道具赋予的能力")
	TObjectPtr<UCancerAbilitySet> AbilitySetToGrant;
};
