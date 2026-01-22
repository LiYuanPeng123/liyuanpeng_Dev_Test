#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "InputModeAwareInterface.generated.h"


UINTERFACE()
class UInputModeAwareInterface : public UInterface
{
	GENERATED_BODY()
};


class CANCERINPUT_API IInputModeAwareInterface
{
	GENERATED_BODY()

public:
	/**
	 * Provides the Input Mode currently in use.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input Mode Aware Interface")
	FGameplayTag GetPlayerInputMode() const;

	/**
	 * Provides a new Input Mode.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input Mode Aware Interface")
	void SetPlayerInputMode(FGameplayTag InputModeTag);
};
