

#include "Input/CanerInputProcessor.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "Widgets/SViewport.h"

UCanerInputProcessor::UCanerInputProcessor()
{
	UpdateInputDeviceType(FKeyEvent());
	PressedKeys = TSet<FKey>();
}

void UCanerInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{	if (FSlateApplication::IsInitialized())
 	{
 		const FVector2D CurMouseLocation = FSlateApplication::Get().GetCursorPos();
 		const FVector2D Dist = CurMouseLocation - LastMouseLocation;
 		if (FMath::Abs(Dist.X) > MouseMoveThreshold || FMath::Abs(Dist.Y) > MouseMoveThreshold)
 		{
 			LastMouseLocation = CurMouseLocation;
 			SetInputDeviceType(EInputDeviceType::EDY_MOUSE);
 			MouseMoveDelegate.Broadcast();
 		}
 	}
 }


bool UCanerInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	UpdateInputDeviceType(InKeyEvent);
	InputKeyDown.Broadcast(InKeyEvent);
	return false;
}

bool UCanerInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	UpdateInputDeviceType(InKeyEvent);
	InputKeyUp.Broadcast(InKeyEvent);
	return false;
}

bool UCanerInputProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp,
	const FAnalogInputEvent& InAnalogInputEvent)
{
	if (InAnalogInputEvent.GetAnalogValue() > GamepadAxisThreshold)
	{
		UpdateInputDeviceType(InAnalogInputEvent);
	}
	return false;
}

bool UCanerInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	SetInputDeviceType(EInputDeviceType::EDY_MOUSE);
	MouseDownMoveDelegate.Broadcast(MouseEvent, MouseEvent.GetPressedButtons().Difference(PressedKeys));
	return false;
}

bool UCanerInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	SetInputDeviceType(EInputDeviceType::EDY_MOUSE);
	const TSet<FKey> DownKeys = MouseEvent.GetPressedButtons().Difference(PressedKeys);
	MouseDownDelegate.Broadcast(MouseEvent, DownKeys);
	PressedKeys = MouseEvent.GetPressedButtons();
	return false;
}

bool UCanerInputProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	SetInputDeviceType(EInputDeviceType::EDY_MOUSE);
	const TSet<FKey> UpKeys = PressedKeys.Difference(MouseEvent.GetPressedButtons());
	MouseUpDelegate.Broadcast(MouseEvent, UpKeys);
	PressedKeys = MouseEvent.GetPressedButtons();
	return false;
}

bool UCanerInputProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp,
	const FPointerEvent& MouseEvent)
{
	SetInputDeviceType(EInputDeviceType::EDY_MOUSE);
	return false;
}

bool UCanerInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp,
	const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent)
{
	SetInputDeviceType(EInputDeviceType::EDY_MOUSE);
	return false;
}

bool UCanerInputProcessor::HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent)
{
	SetInputDeviceType(EInputDeviceType::EDY_KEYBOARD);
	return false;
}

void UCanerInputProcessor::UpdateInputDeviceType(const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey().IsGamepadKey())
	{
		FInputDeviceId InputDeviceId = InKeyEvent.GetInputDeviceId();
		FString DeviceName;
		if (UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
		{
			auto DeviceIdentifier = InputDeviceSubsystem->GetInputDeviceHardwareIdentifier(InputDeviceId);
			DeviceName = DeviceIdentifier.HardwareDeviceIdentifier.ToString();
		}
		if (DeviceName == "DualSense" || DeviceName == "DualShock4")
		{
			SetInputDeviceType(EInputDeviceType::EDY_GAMEPAD_PS);
			return;
		}
		if (DeviceName == "XInputController")
		{
			SetInputDeviceType(EInputDeviceType::EDY_GAMEPAD_XBOX);
			return;
		}
		SetInputDeviceType(EInputDeviceType::EDY_GAMEPAD_XBOX);
	}
	else
	{
		SetInputDeviceType(EInputDeviceType::EDY_KEYBOARD);
	}
}

void UCanerInputProcessor::SetInputDeviceType(EInputDeviceType NewInputDeviceType)
{
	if (NewInputDeviceType != InputDeviceType)
    	{
    		//应用程序是激活的，并且焦点在视口
    		if (FSlateApplication::Get().IsActive())
    		{
    			TSharedPtr<FSlateUser> SlateUser = FSlateApplication::Get().GetUser(0);
    			TSharedPtr<SViewport> GameViewport = FSlateApplication::Get().GetGameViewport();
    			if (SlateUser && GameViewport)
    			{
    				if (SlateUser->IsWidgetInFocusPath(GameViewport))
    				{
    					InputDeviceType = NewInputDeviceType;
    					//通知
    					if (InputDeviceTypeChangeDelegate.IsBound())
    					{
    						InputDeviceTypeChangeDelegate.Broadcast(InputDeviceType);
    					}
    				}
    			}
    		}
    	}
}
