#include "InputHandlers/InputHandler_InputAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerInputValuePayload.h"
#include "CancerNativeGamePlayTag.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/CancerInputManagerComponent.h"
#include "Data/CancerAbilityInputConfig.h"


DEFINE_LOG_CATEGORY_STATIC(LogInputAbility, Log, All);

UInputHandler_InputAbility::UInputHandler_InputAbility()
{
	InputActions.Empty();
	TriggerEvents.Empty();
	TriggerEvents.Add(ETriggerEvent::Triggered);
	TriggerEvents.Add(ETriggerEvent::Started);
	TriggerEvents.Add(ETriggerEvent::Completed);
	InputValuePayload = CreateOptionalDefaultSubobject<UCancerInputValuePayload>("InputValuePayload");
}

void UInputHandler_InputAbility::HandleTriggeredEvent_Implementation(UCancerInputManagerComponent* Manager,
                                                                     const FInputActionValue& Value,
                                                                     const UInputAction* InputAction,
                                                                     float ElapsedTime) const
{
	Super::HandleTriggeredEvent_Implementation(Manager, Value, InputAction, ElapsedTime);
	if (auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Manager->GetPawn()))
	{
		Manager->AbilityInputTagTriggered(FindAbilityTagForInputAction(InputAction));
		FGameplayEventData EventData;
		EventData.EventTag = Tag_Combat_Event_Input_Trigger;
		EventData.OptionalObject = InputAction;
		InputValuePayload->InputValue = Value;
		InputValuePayload->InputTagContainer = FindAbilityTagForInputAction(InputAction);
		EventData.OptionalObject2 = InputValuePayload;
		ASC->HandleGameplayEvent(Tag_Combat_Event_Input_Trigger, &EventData);
	}
}

void UInputHandler_InputAbility::HandleStartedEvent_Implementation(UCancerInputManagerComponent* Manager,
                                                                   const FInputActionValue& Value,
                                                                   const UInputAction* InputAction) const
{
	Super::HandleStartedEvent_Implementation(Manager, Value, InputAction);
	if (auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Manager->GetPawn()))
	{

		Manager->AbilityInputTagPressed(FindAbilityTagForInputAction(InputAction));
		FGameplayEventData EventData;
		EventData.EventTag = Tag_Combat_Event_Input_Start;
		EventData.OptionalObject = InputAction;
		InputValuePayload->InputValue = Value;
		InputValuePayload->InputTagContainer = FindAbilityTagForInputAction(InputAction);
		EventData.OptionalObject2 = InputValuePayload;
		ASC->HandleGameplayEvent(Tag_Combat_Event_Input_Start, &EventData);
	}
}

void UInputHandler_InputAbility::HandleCompletedEvent_Implementation(UCancerInputManagerComponent* Manager,
                                                                     const FInputActionValue& Value,
                                                                     const UInputAction* InputAction) const
{
	Super::HandleCompletedEvent_Implementation(Manager, Value, InputAction);
	if (auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Manager->GetPawn()))
	{
		Manager->AbilityInputTagReleased(FindAbilityTagForInputAction(InputAction));
		FGameplayEventData EventData;
		EventData.EventTag = Tag_Combat_Event_Input_Completed;
		EventData.OptionalObject = InputAction;
		InputValuePayload->InputValue = Value;
		InputValuePayload->InputTagContainer = FindAbilityTagForInputAction(InputAction);
		EventData.OptionalObject2 = InputValuePayload;
		ASC->HandleGameplayEvent(Tag_Combat_Event_Input_Completed, &EventData);
	}
}


bool UInputHandler_InputAbility::CanBeBuffered(const UInputAction* InInputAction) const
{
	if (!bCanBeBuffered) return false;

	for (auto It : AbilityInputActions)
	{
		if (It.InputAction == InInputAction)
		{
			return It.bCanBuffer;
		}
	}
	return false;
}

const UInputAction* UInputHandler_InputAbility::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
                                                                             bool bLogNotFound) const
{
	for (const FCancerInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag.HasTag(InputTag)))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogInputAbility, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."),
		       *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

FGameplayTagContainer UInputHandler_InputAbility::FindAbilityTagForInputAction(const UInputAction* InputAction,
                                                                               bool bLogNotFound) const
{
	for (const FCancerInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction == InputAction)
		{
			return Action.InputTag;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogInputAbility, Error, TEXT("Can't find AbilityInputAction for InputAction [%s] on InputConfig [%s]."),
		       *InputAction->GetName(), *GetNameSafe(this));
	}

	return FGameplayTagContainer::EmptyContainer;
}
