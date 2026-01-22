
#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "InventoryFragment_SetStats.generated.h"

class UInventoryItemInstance;


UCLASS(DisplayName="物品消耗")
class CANCERINVENTORY_API UInventoryFragment_SetStats : public UInventoryItemFragment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TMap<FGameplayTag, int32> InitialItemStats;

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const override;

	int32 GetItemStatByTag(FGameplayTag Tag) const;
};
