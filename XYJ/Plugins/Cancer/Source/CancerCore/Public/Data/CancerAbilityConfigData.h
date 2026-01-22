#pragma once
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "CancerAbilityConfigData.generated.h"

USTRUCT(BlueprintType)
struct FCancerAbilityConfigData : public FTableRowBase
{
	GENERATED_BODY()
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag AbilityTriggerTag;

};