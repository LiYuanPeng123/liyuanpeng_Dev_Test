
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CancerWidgetEntry.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class CANCERABILITY_API UCancerWidgetEntry : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(BlueprintReadOnly)
	AActor* OwnerActor;
};
