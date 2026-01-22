#pragma once

#include "CoreMinimal.h"
#include "CancerInputHandler.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "InputHandler_InputAbility.generated.h"

class UCancerInputValuePayload;
struct FCancerInputAction;
struct FInputActionValue;
class UCancerAbilityInputConfig;



/**
 * 
 */
UCLASS(DisplayName="技能映射")
class CANCERINPUT_API UInputHandler_InputAbility : public UCancerInputHandler
{
	GENERATED_BODY()

public:
	UInputHandler_InputAbility();
	

	virtual void HandleTriggeredEvent_Implementation(UCancerInputManagerComponent* Manager,
	                                                 const FInputActionValue& Value, const UInputAction* InputAction,
	                                                 float ElapsedTime) const override;
	virtual void HandleStartedEvent_Implementation(UCancerInputManagerComponent* Manager,
	                                               const FInputActionValue& Value,
	                                               const UInputAction* InputAction) const override;
	virtual void HandleCompletedEvent_Implementation(UCancerInputManagerComponent* Manager,
	                                                 const FInputActionValue& Value,
	                                                 const UInputAction* InputAction) const override;

	UPROPERTY(BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FCancerInputAction> AbilityInputActions;

	virtual bool CanBeBuffered(const UInputAction* InInputAction) const override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Cancer")
	FGameplayTagContainer FindAbilityTagForInputAction(const UInputAction* InputAction, bool bLogNotFound = true) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Cancer|Input", DisplayName="输入数据")
	UCancerInputValuePayload* InputValuePayload;
};
