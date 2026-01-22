#include"Data/CancerInputConfig.h"
#include "CancerInputHandler.h"
#include "Data/CancerInputSetupDataValidator.h"
#include "InputHandlers/InputHandler_InputAbility.h"

UCancerInputConfig::UCancerInputConfig()
{
	Priority = 0;
	InputHandler = CreateOptionalDefaultSubobject<UInputHandler_InputAbility>("InputHandler");
}

void UCancerInputConfig::InitInput() const 
{
	InputHandler->AbilityInputActions = GetInputInfo();
	for (auto InputTemp:InputHandler->AbilityInputActions)
	{
		InputHandler->InputActions.AddUnique(InputTemp.InputAction);
		InputHandler->bCanBeBuffered = bCanBeBuffered;
		InputHandler->BufferChannelTag = BufferChannelTag;
	}
}

FPrimaryAssetId UCancerInputConfig::GetPrimaryAssetId() const
{
	const FPrimaryAssetType BaseAssetType = TEXT("InputSetup");
	return FPrimaryAssetId(BaseAssetType, GetFName());
}

bool UCancerInputConfig::HasCompatibleInputHandler(const UInputAction* InputAction,
                                                   const ETriggerEvent& TriggerEvent) const
{
	return IsValid(InputHandler) && InputHandler->CanHandle(TriggerEvent, InputAction);
}

#if WITH_EDITOR
EDataValidationResult UCancerInputConfig::IsDataValid(FDataValidationContext& Context)
{
	return FCancerInputSetupDataValidator::ValidateInputSetupData(this, Context);
}
#endif
