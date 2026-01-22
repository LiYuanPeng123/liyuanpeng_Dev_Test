#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CancerPickupActor.h"
#include "CancerInventorySetting.generated.h"


UCLASS(config = Game, DefaultConfig, DisplayName="CancerInventory")
class CANCERINVENTORY_API UCancerInventorySetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditDefaultsOnly,
		DisplayName="道具配置表", Category = Inventory, meta=(
			RequiredAssetDataTags = "RowStructure=/Script/CancerInventory.InventoryItemInfo"))
	TArray<TSoftObjectPtr<UDataTable>> ItemDataTables;

	UPROPERTY(Config, EditDefaultsOnly, DisplayName="交互类")
	TSubclassOf<ACancerPickupActor> PickupActorClass;

	UPROPERTY(EditDefaultsOnly, DisplayName="默认交互定义")
	UCancerInteractionDefinition* PickupDefinition;
	
	static UCancerInventorySetting* Get();
	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;


};
