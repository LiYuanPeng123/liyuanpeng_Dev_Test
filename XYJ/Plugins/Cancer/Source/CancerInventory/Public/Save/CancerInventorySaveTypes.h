#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CancerInventorySaveTypes.generated.h"

USTRUCT(BlueprintType)
struct CANCERINVENTORY_API FInventorySaveEntry
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ItemTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackCount = 0;
};

USTRUCT(BlueprintType)
struct CANCERINVENTORY_API FQuickBarSave
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag QuickBarTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> SlotsTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ActiveSlotIndex = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumSlots = 0;
};
