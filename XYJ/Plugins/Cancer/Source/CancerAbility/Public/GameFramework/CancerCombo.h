
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "CancerCombo.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS()
class CANCERABILITY_API UCancerCombo : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag ComboTag;
};

UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType)
class CANCERABILITY_API UCancerSpecialAbility : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag ComboTag;
};
