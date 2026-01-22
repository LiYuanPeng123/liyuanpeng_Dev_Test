
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "CancerInputHandler.generated.h"

struct FInputActionInstance;
class UInputAction;
enum class ETriggerEvent : uint8;
class UCancerInputManagerComponent;
struct FGameplayTagContainer;


UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew, ClassGroup = CancerInput)
class CANCERINPUT_API UCancerInputHandler : public UObject
{
	GENERATED_BODY()
	public:

    UCancerInputHandler();

    // -- Begin Object implementation
    virtual UWorld* GetWorld() const override;
    // -- End Object implementation


	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler")
	bool CanHandle(const ETriggerEvent& TriggerEvent, const UInputAction* InputAction) const;
    

	virtual  void HandleInput(UCancerInputManagerComponent* Manager, const FInputActionInstance& ActionInstance,
		const ETriggerEvent& TriggerEvent) const;

	UFUNCTION(BlueprintPure, Category = "Cancer Input|Handler")
	FGameplayTag GetBufferChannelTag()const;
	
    UFUNCTION(BlueprintPure,Category = "Cancer Input|Handler")
    virtual bool CanBeBuffered(const UInputAction* InInputAction) const;
	
    void SetWorld(UWorld* WorldReference);

	/** Actions that may trigger this input. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Handler")
	TArray<TObjectPtr<UInputAction>> InputActions;

	/** Informs if this handler can be buffered. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Handler")
	bool bCanBeBuffered;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Handler", meta = (EditCondition = "bCanBeBuffered"))
	FGameplayTag BufferChannelTag;
	
	/** Triggers that will invoke the handler. */
    	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Handler")
    	TArray<ETriggerEvent> TriggerEvents;
protected:

	

	
	
	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler")
	void HandleStartedEvent(UCancerInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler")
	void HandleTriggeredEvent(UCancerInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction, float ElapsedTime) const;
	

	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler")
	void HandleOngoingEvent(UCancerInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction, float ElapsedTime) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler")
	void HandleCompletedEvent(UCancerInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler")
	void HandleCancelledEvent(UCancerInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const;
	
	UFUNCTION(BlueprintPure, Category = "Input Handler")
	static bool HasAnyTags(const UCancerInputManagerComponent* Manager, const FGameplayTagContainer& Tags);

private:
	
    TWeakObjectPtr<UWorld> WorldPtr;
    
    void AddOnScreenDebugMessage(const UCancerInputManagerComponent* Manager, const FInputActionInstance& ActionInstance,
        const ETriggerEvent& TriggerEvent) const;

#if WITH_EDITOR
public:
	
    /** Checks if this handler has any Input Action. */
    bool HasAnyInputActions() const;

    /** Checks if this handler has any Trigger Events. */
    bool HasAnyTriggerEvents() const;
	
    /** Checks if this Handler is compatible with a certain action. */
    bool HandlesAction(const UInputAction* InputAction) const;
#endif
};
