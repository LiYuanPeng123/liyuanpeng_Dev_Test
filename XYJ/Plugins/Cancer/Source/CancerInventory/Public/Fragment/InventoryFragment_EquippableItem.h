#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDefinition.h"

#include "InventoryFragment_EquippableItem.generated.h"

class UEquipmentDefinition;
/**
 * 
 */
UCLASS(DisplayName="装备定义")
class CANCERINVENTORY_API UInventoryFragment_EquippableItem : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer, DisplayName="装备定义")
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;
};
