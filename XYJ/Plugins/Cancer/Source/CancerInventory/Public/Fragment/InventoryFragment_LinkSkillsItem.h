
#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDefinition.h"
#include "InventoryFragment_LinkSkillsItem.generated.h"

/**
 * 
 */
UCLASS(DisplayName="功法(携带武技)")
class CANCERINVENTORY_API UInventoryFragment_LinkSkillsItem : public UInventoryItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<FGameplayTag,FGameplayTagContainer> LinkItemTags;
};
