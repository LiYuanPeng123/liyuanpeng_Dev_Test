
#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDefinition.h"
#include "InventoryFragment_SetCount.generated.h"

/**
 * 
 */
UCLASS(DisplayName="物品数量")
class CANCERINVENTORY_API UInventoryFragment_SetCount : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category=Equipment)
	EInventoryItemStack ItemStack{EInventoryItemStack::None};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Equipment,
		meta=(EditCondition ="ItemStack != EInventoryItemStack::None"))
	int32 StackLimit = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Equipment,
		meta=(EditCondition ="ItemStack != EInventoryItemStack::None"))
	int32 MaximumLimit = 1;
};
