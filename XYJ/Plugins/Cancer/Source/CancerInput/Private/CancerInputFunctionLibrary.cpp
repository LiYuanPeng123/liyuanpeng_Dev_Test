

#include "CancerInputFunctionLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CancerInputManagerComponent.h"
#include "Types/FBufferedInputCommand.h"

FBufferedInputCommand UCancerInputFunctionLibrary::MakeBufferedInputCommand(UCancerInputManagerComponent* Source,
                                                                            UCancerInputHandler* Handler, const FInputActionInstance& ActionInstance, ETriggerEvent TriggerEvent)
{
	return FBufferedInputCommand(Source, Handler, ActionInstance, TriggerEvent);
}

void UCancerInputFunctionLibrary::BreakBufferedInputCommand(const FBufferedInputCommand& Command, bool& bIsValid,
	UCancerInputManagerComponent*& Source, UCancerInputHandler*& Handler, FInputActionInstance& ActionInstance,
	ETriggerEvent& TriggerEvent)
{
	bIsValid = Command.IsValid();
	Source = Command.GetSource();
	Handler = Command.GetHandler();
	TriggerEvent = Command.GetTriggerEvent();
	
	const FInputActionInstance* OriginalInstance = Command.GetActionInstance();
	ActionInstance = OriginalInstance ? *OriginalInstance : FInputActionInstance();
}

class UInputComponent* UCancerInputFunctionLibrary::K2_GetInputComponent(AActor* Actor, bool& bIsValid,
	TSubclassOf<UInputComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetInputComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UInputComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

class UCancerInputManagerComponent* UCancerInputFunctionLibrary::K2_CancerInputManagerComponent(AActor* Actor, bool& bIsValid,
	TSubclassOf<UCancerInputManagerComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerInputManagerComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UInputComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

UActorComponent* UCancerInputFunctionLibrary::GetInputBufferComponent(const AActor* Actor)
{
	if (IsValid(Actor))
	{
		const TArray<UActorComponent*> Components = Actor->GetComponentsByInterface(UInputBufferInterface::StaticClass());

		const int32 Idx = Components.IndexOfByPredicate([](const UActorComponent* Candidate)
			{ return IsValid(Candidate) && IInputBufferInterface::Execute_IsInputBufferEnabled(Candidate); });

		return Idx != INDEX_NONE ? Components[Idx] : nullptr;
	}

	return nullptr;
}

FInputActionValue UCancerInputFunctionLibrary::GetInputActionValue(UObject* WorldContextObject,
                                                                   UInputAction* InputAction)
{
	if (!WorldContextObject) return FInputActionValue();
	const ULocalPlayer* LocalPlayer = WorldContextObject->GetWorld()->GetFirstPlayerController()->GetLocalPlayer();
	if (const UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		const UEnhancedPlayerInput* PlayerInput = Subsystem->GetPlayerInput();
		return PlayerInput->GetActionValue(InputAction);
	}
	return FInputActionValue();
}

FInputActionInstance UCancerInputFunctionLibrary::GetActionInstance(UObject* WorldContextObject,
	UInputAction* InputAction)
{
	if (!WorldContextObject) return FInputActionInstance();
	if (!WorldContextObject->GetWorld()) return FInputActionInstance();
	if (!WorldContextObject->GetWorld()->GetFirstPlayerController())return FInputActionInstance();
	const ULocalPlayer* LocalPlayer = WorldContextObject->GetWorld()->GetFirstPlayerController()->GetLocalPlayer();
	if (const UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		const UEnhancedPlayerInput* PlayerInput = Subsystem->GetPlayerInput();
		if ( auto Ins = PlayerInput->FindActionInstanceData(InputAction))
		return *Ins;
	}
	return FInputActionInstance();
}
