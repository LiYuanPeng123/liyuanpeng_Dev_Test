
#include "CancerUserWidget.h"
#include "Common/CancerUIManagerSubsystem.h"

#define LOCTEXT_NAMESPACE "Cancer"
void UCancerUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (UCancerUIManagerSubsystem* UISubsystem = GetWorld()->GetSubsystem<UCancerUIManagerSubsystem>())
	{
		const EInputDeviceType CurInputDeviceType = UISubsystem->GetInputDeviceType();
		ReceiveInputDeviceChange(CurInputDeviceType);
		UISubsystem->InputDeviceTypeChangeDelegate.AddUniqueDynamic(this, &UCancerUserWidget::ReceiveInputDeviceChange);
	}
}

void UCancerUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (UCancerUIManagerSubsystem* UISubsystem = GetWorld()->GetSubsystem<UCancerUIManagerSubsystem>())
	{
		UISubsystem->InputDeviceTypeChangeDelegate.RemoveDynamic(this, &UCancerUserWidget::ReceiveInputDeviceChange);
	}
}

void UCancerUserWidget::HandleReceviedData_Implementation(UObject* Object)
{
}

void UCancerUserWidget::OnOpened_Implementation()
{
}

void UCancerUserWidget::OnClosed_Implementation()
{
	OnClose.Broadcast();
}
bool UCancerUserWidget::ReceiveActionEvent_Implementation(const UInputAction* InputAction, ETriggerEvent TriggerEvent)
{
	if (!bEnableInput)
	{
		return false;
	}
	return InputAction && TriggerEvent != ETriggerEvent::None;
}


#undef LOCTEXT_NAMESPACE


