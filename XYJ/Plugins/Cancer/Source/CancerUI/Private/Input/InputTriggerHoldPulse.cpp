

/*
#include "Input/InputTriggerHoldPulse.h"

ETriggerState UInputTriggerHoldPulse::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput,
	FInputActionValue ModifiedValue, float DeltaTime)
{
	ETriggerState State = Super::UpdateState_Implementation(PlayerInput, ModifiedValue, DeltaTime);
	if (State == ETriggerState::Ongoing)
	{
		switch (TriggerHoldPulseState)
		{
		case ETriggerHoldPulseState::Hold:
			{
				TriggerHoldPulseState = ETriggerHoldPulseState::Pulse;
				State = ETriggerState::Triggered;
			}
			break;
		case ETriggerHoldPulseState::Pulse:
			{
				if (HeldDuration > PulseStartTime + PulseIntervalTime * TriggerCount)
				{
					++TriggerCount;
					State = ETriggerState::Triggered;
				}
			}
			break;
		default: ;
		}
	}
	else
	{
		TriggerCount = 0;
		TriggerHoldPulseState = ETriggerHoldPulseState::Hold;
	}
	return State;
}
*/
