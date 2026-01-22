
#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "FBufferedInputCommand.generated.h"

class UCancerInputManagerComponent;
class UCancerInputHandler;
struct FInputActionInstance;



USTRUCT(BlueprintType,meta = (
	HasNativeBreak = "/Script/CancerInput.CancerInputFunctionLibrary.BreakBufferedInputCommand",
	HasNativeMake = "/Script/CancerInput.CancerInputFunctionLibrary.MakeBufferedInputCommand")
)
struct FBufferedInputCommand
{
    
    GENERATED_BODY()

private:
	
	UPROPERTY()
    TObjectPtr<UCancerInputManagerComponent> Source;
	
	UPROPERTY()
    TObjectPtr<UCancerInputHandler> Handler;
	
    TSharedPtr<FInputActionInstance> ActionInstancePtr;
	
    ETriggerEvent TriggerEvent;

public:
	
    FBufferedInputCommand()
    {
    	Source = nullptr;
        Handler = nullptr;
    	ActionInstancePtr.Reset();
        TriggerEvent = ETriggerEvent::None;
    }

    explicit FBufferedInputCommand(UCancerInputManagerComponent* Source
        , UCancerInputHandler* Handler
        , const FInputActionInstance& ActionInstance
        , const ETriggerEvent TriggerEvent)
        : Source(Source)
        , Handler(Handler)
        , TriggerEvent(TriggerEvent)
    {
    	ActionInstancePtr = MakeShared<FInputActionInstance>(ActionInstance);
    }

	~FBufferedInputCommand()
    {
		ActionInstancePtr.Reset();    
    }
	
  
    bool IsValid() const;
    
   
    void Execute() const;

	
	UCancerInputManagerComponent* GetSource() const;

	
	UCancerInputHandler* GetHandler() const;

    const FInputActionInstance* GetActionInstance() const;


	const UInputAction* GetSourceAction() const;
	

	ETriggerEvent GetTriggerEvent() const;
	

    FORCEINLINE bool operator == (const FBufferedInputCommand& In) const
    {
        return In.ActionInstancePtr->GetSourceAction() == ActionInstancePtr->GetSourceAction()
    		&& In.Handler == Handler && In.TriggerEvent == TriggerEvent;
    }
	
    FORCEINLINE bool operator != (const FBufferedInputCommand& In) const
    {
        return In.ActionInstancePtr->GetSourceAction() != ActionInstancePtr->GetSourceAction()
    		|| In.Handler != Handler
    		|| In.TriggerEvent != TriggerEvent;
    }
};