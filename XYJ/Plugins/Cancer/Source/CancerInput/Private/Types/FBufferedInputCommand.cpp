#include "Types/FBufferedInputCommand.h"

#include "CancerInputHandler.h"
#include "InputAction.h"


bool FBufferedInputCommand::IsValid() const
{
	return Source != nullptr
		&& ActionInstancePtr.IsValid()
		&& Handler != nullptr
		&& TriggerEvent != ETriggerEvent::None;
}

void FBufferedInputCommand::Execute() const
{
	if (IsValid())
	{
		Handler->HandleInput(Source, *ActionInstancePtr.Get(), TriggerEvent);
	}
}

UCancerInputManagerComponent* FBufferedInputCommand::GetSource() const
{
	return Source;
}

UCancerInputHandler* FBufferedInputCommand::GetHandler() const
{
	return Handler;
}

const FInputActionInstance* FBufferedInputCommand::GetActionInstance() const
{
	if (ActionInstancePtr.IsValid())
	{
		return ActionInstancePtr.Get();
	}

	return nullptr;
}

const UInputAction* FBufferedInputCommand::GetSourceAction() const
{
	if (ActionInstancePtr.IsValid())
	{
		return ActionInstancePtr->GetSourceAction();
	}

	return nullptr;
}

ETriggerEvent FBufferedInputCommand::GetTriggerEvent() const
{
	return TriggerEvent;
}
