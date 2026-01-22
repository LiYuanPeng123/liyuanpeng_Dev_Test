#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CancerInventorySubsystem.generated.h"

class UCancerInteractionDefinition;
class UInventoryItemDefinition;

UCLASS()
class CANCERINVENTORY_API UCancerInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	TSubclassOf<UInventoryItemDefinition> FindInventoryDef(FGameplayTag ItemTag);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	UCancerInteractionDefinition* FindInventoryPickupDef(FGameplayTag ItemTag);

	UPROPERTY()
	TMap<FGameplayTag, TSubclassOf<UInventoryItemDefinition>> InventoryItemMap;

	UPROPERTY()
	TMap<FGameplayTag, UCancerInteractionDefinition*> InventoryItemPickDefMap;

	UPROPERTY()
	TMap<int32, FGameplayTag> InventoryItemIdToTag;
};
