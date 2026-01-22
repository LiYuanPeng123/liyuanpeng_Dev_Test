
#include "CancerInputHandler.h"
#include "AbilitySystemComponent.h"
#include "CancerInputLog.h"
#include "CancerInputSettings.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "GameFramework/Controller.h"
#include "Components/CancerInputManagerComponent.h"

UCancerInputHandler::UCancerInputHandler()
{
	bCanBeBuffered = false;
}

UWorld* UCancerInputHandler::GetWorld() const
{
	return WorldPtr.IsValid() && WorldPtr->IsValidLowLevelFast() ? WorldPtr.Get() : nullptr;
}

bool UCancerInputHandler::CanHandle_Implementation(const ETriggerEvent& TriggerEvent,
	const UInputAction* InputAction) const
{
	return TriggerEvent != ETriggerEvent::None && IsValid(InputAction) &&
		InputActions.Contains(InputAction) && TriggerEvents.Contains(TriggerEvent);
}

void UCancerInputHandler::HandleInput(UCancerInputManagerComponent* Manager, const FInputActionInstance& ActionInstance,
	const ETriggerEvent& TriggerEvent) const
{
	check(IsValid(Manager));

	const FInputActionValue Value = ActionInstance.GetValue();
	const TObjectPtr<const UInputAction> InputAction = ActionInstance.GetSourceAction();
	const float ElapsedTime = ActionInstance.GetElapsedTime();

	AddOnScreenDebugMessage(Manager, ActionInstance, TriggerEvent);
    
	switch (TriggerEvent)
	{
	case ETriggerEvent::Started:
		HandleStartedEvent(Manager, Value, InputAction);
		break;
	case ETriggerEvent::Triggered:
		HandleTriggeredEvent(Manager, Value, InputAction, ElapsedTime);
		break;
	case ETriggerEvent::Ongoing:
		HandleOngoingEvent(Manager, Value, InputAction, ElapsedTime);
		break;
	case ETriggerEvent::Completed:
		HandleCompletedEvent(Manager, Value, InputAction);
		break;
	case ETriggerEvent::Canceled:
		HandleCancelledEvent(Manager, Value, InputAction);
		break;
	default:
		const UEnum* EnumPtr = FindObject<UEnum>(GetOuter(), TEXT("ETriggerEvent"), true);
		UE_LOG(CancerInputLog, Warning, TEXT("[%s] Unable to handle Trigger Event '%s'."),
			*GetNameSafe(Manager->GetOwner()), *EnumPtr->GetValueAsString(TriggerEvent));
	}
}

FGameplayTag UCancerInputHandler::GetBufferChannelTag() const
{
	return BufferChannelTag;
}

bool UCancerInputHandler::CanBeBuffered(const UInputAction* InInputAction) const
{
	return bCanBeBuffered;
}


void UCancerInputHandler::SetWorld(UWorld* WorldReference)
{
	WorldPtr = WorldReference;
}

void UCancerInputHandler::HandleStartedEvent_Implementation(UCancerInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
}

void UCancerInputHandler::HandleTriggeredEvent_Implementation(UCancerInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction, float ElapsedTime) const
{
}

void UCancerInputHandler::HandleOngoingEvent_Implementation(UCancerInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction, float ElapsedTime) const
{
}

void UCancerInputHandler::HandleCompletedEvent_Implementation(UCancerInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
}

void UCancerInputHandler::HandleCancelledEvent_Implementation(UCancerInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction) const
{
}

bool UCancerInputHandler::HasAnyTags(const UCancerInputManagerComponent* Manager, const FGameplayTagContainer& Tags)
{
	check(Manager);

	if (Tags.IsEmpty() == false)
	{
		const UAbilitySystemComponent* AbilityComponent = Manager->GetAbilitySystemComponent();
		return IsValid(AbilityComponent) && AbilityComponent->HasAnyMatchingGameplayTags(Tags);
	}

	return false;
}

void UCancerInputHandler::AddOnScreenDebugMessage(const UCancerInputManagerComponent* Manager,
	const FInputActionInstance& ActionInstance, const ETriggerEvent& TriggerEvent) const
{
#if WITH_EDITOR
	if (IsValid(GEngine) && GetDefault<UCancerInputSettings>()->bShowScreenDebugMessages)
	{
		const FInputActionValue Value = ActionInstance.GetValue();
		const TObjectPtr<const UInputAction> InputAction = ActionInstance.GetSourceAction();
		const float ElapsedTime = ActionInstance.GetElapsedTime();
        
		const UEnum* EnumPtr = FindObject<UEnum>(GetOuter(), TEXT("ETriggerEvent"), true);
		const FString DebugMessage = FString::Printf(TEXT("Input Handler Debug: { Class: '%s', Owner: '%s', Action: '%s', Trigger: '%s', Value: '%s', Elapsed Time: %f }"),
			*GetNameSafe(this), *GetNameSafe(Manager->GetOwner()), *GetNameSafe(InputAction), *EnumPtr->GetValueAsString(TriggerEvent), *Value.ToString(), ElapsedTime);

		const float Duration = GetDefault<UCancerInputSettings>()->DebugMessageDuration;
		const FColor DebugColor = GetDefault<UCancerInputSettings>()->DebugMessageColor;
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, Duration, DebugColor, DebugMessage);
	}
#endif
}


#if WITH_EDITOR
bool UCancerInputHandler::HasAnyInputActions() const
{
	return !InputActions.IsEmpty();
}

bool UCancerInputHandler::HasAnyTriggerEvents() const
{
	return !TriggerEvents.IsEmpty();
}

bool UCancerInputHandler::HandlesAction(const UInputAction* InputAction) const
{
	return InputActions.Contains(InputAction);
}
#endif