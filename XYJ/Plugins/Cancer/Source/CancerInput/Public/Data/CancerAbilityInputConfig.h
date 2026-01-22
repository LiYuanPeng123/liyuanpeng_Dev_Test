
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "CancerAbilityInputConfig.generated.h"


class UInputAction;

USTRUCT(BlueprintType)
struct FCancerInputAction:public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanBuffer = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta=(Categories="Input"))
	FGameplayTagContainer InputTag;
	
};

