#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Save/CancerInventorySaveTypes.h"
#include "CancerInventorySaveGame.generated.h"

UCLASS()
class CANCERINVENTORY_API UCancerInventorySaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInventorySaveEntry> InventoryEntries;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FQuickBarSave> QuickBars;
};
