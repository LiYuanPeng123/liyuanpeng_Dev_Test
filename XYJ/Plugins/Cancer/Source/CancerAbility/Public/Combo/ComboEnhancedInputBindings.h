#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ComboEnhancedInputBindings.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FComboEnhancedBinding
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag PhysicalTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsAxis = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AxisTag;
};

UCLASS(BlueprintType)
class CANCERABILITY_API UComboEnhancedInputBindings : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MappingPriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FComboEnhancedBinding> Bindings;
};

