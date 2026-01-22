#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "CancerInputValuePayload.generated.h"

/**
 * 
 */
UCLASS()
class CANCERINPUT_API UCancerInputValuePayload : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FInputActionValue InputValue;
	UPROPERTY()
	FInputActionInstance InputActionIns;
	UPROPERTY()
	FGameplayTagContainer InputTagContainer;
};
