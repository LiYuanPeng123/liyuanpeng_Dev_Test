#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDefinition.h"
#include "InventoryItemFragment_ItemEffect.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FInventoryItemFragment_Effect
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category= Effect)
	TSubclassOf<UGameplayEffect> ItemEffect;
	UPROPERTY(EditAnywhere, Category= Effect)
	int32 EffectLevel = 1;
};


/**
 * 
 */
UCLASS(DisplayName="道具效果")
class CANCERINVENTORY_API UInventoryItemFragment_ItemEffect : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category= Effect)
	TArray<FInventoryItemFragment_Effect> Effects;
};
