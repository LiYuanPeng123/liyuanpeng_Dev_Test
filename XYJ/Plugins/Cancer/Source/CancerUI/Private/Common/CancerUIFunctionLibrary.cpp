

#include "Common/CancerUIFunctionLibrary.h"

#include "AudioDeviceManager.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Common/CancerUIPolicy.h"
#include "Common/CancerLayout.h"
#include "Common/CancerUIManagerSubsystem.h"



UCancerUserWidget* UCancerUIFunctionLibrary::GetUI(UObject* WorldContextObject, FGameplayTag UIName,
	TSubclassOf<UCancerUserWidget> Class)
{
	if (WorldContextObject)
	{
		if (const auto World = WorldContextObject->GetWorld())
		{
			if (UCancerUIManagerSubsystem* UISubsystem = World->GetSubsystem<UCancerUIManagerSubsystem>())

			{
				return UISubsystem->GetUI(UIName);
			}
		}
	}
	return nullptr;
}

UCancerUserWidget* UCancerUIFunctionLibrary::OpenUI(UObject* WorldContextObject, FGameplayTag UIName,
                                               TSubclassOf<UCancerUserWidget> Class)
{
	if (WorldContextObject)
	{
		if (const auto World = WorldContextObject->GetWorld())
		{
			if (UCancerUIManagerSubsystem* UISubsystem = World->GetSubsystem<UCancerUIManagerSubsystem>())

			{
				return UISubsystem->OpenUI(UIName);
			}
		}
	}
	return nullptr;
}

void UCancerUIFunctionLibrary::CloseUI(UObject* WorldContextObject, FGameplayTag UIName, bool bDestroy,
	UCancerUserWidget* InWidget)
{
	if (WorldContextObject)
	{
		if (const auto World = WorldContextObject->GetWorld())
		{
			if (UCancerUIManagerSubsystem* UISubsystem = World->GetSubsystem<UCancerUIManagerSubsystem>())

			{
				UISubsystem->CloseUI(UIName,bDestroy,InWidget);
			}
		}
	}
}

UCancerUserWidget* UCancerUIFunctionLibrary::OpenUIWithData(UObject* WorldContextObject, FGameplayTag UIName,
                                                            TSubclassOf<UCancerUserWidget> Class, UObject* Payload)
{
	if (auto UI = OpenUI(WorldContextObject, UIName, Class))
	{
		UI->HandleReceviedData(Payload);
		return UI;
	}
	return nullptr;
}

UCancerUserWidget* UCancerUIFunctionLibrary::CloseUIWithData(UObject* WorldContextObject, FGameplayTag UIName,
                                                             bool bDestroy, UCancerUserWidget* InWidget,
                                                             UObject* Payload)
{
	if (auto UI = GetUI(WorldContextObject, UIName, nullptr))
	{
		UI->HandleReceviedData(Payload);
		CloseUI(WorldContextObject, UIName, bDestroy, InWidget);
		return UI;
	}
	return nullptr;
}


void UCancerUIFunctionLibrary::FocusTargetWidget(UUserWidget* Widget)
{
	if (Widget)
	{
		Widget->SetIsFocusable(true);
		Widget->SetFocus();
		Widget->SetIsFocusable(false);
	}
}

EDirectionType UCancerUIFunctionLibrary::GetWASD(UObject* WorldContextObject,UInputAction* InputAction)
{
	const ULocalPlayer* LocalPlayer = WorldContextObject->GetWorld()->GetFirstPlayerController()->GetLocalPlayer();
	if (const UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		const UEnhancedPlayerInput* PlayerInput = Subsystem->GetPlayerInput();
			FInputActionValue Value = PlayerInput->GetActionValue(InputAction);
		FVector2D Direction =  Value.Get<FVector2D>();
		if (FMath::Abs(Direction.X) >= FMath::Abs(Direction.Y))
		{
			if (Direction.X < 0)
			{
				return  EDirectionType::DT_Left;
			}
			return  EDirectionType::DT_Right;
		}
		if (Direction.Y < 0)
		{
			return  EDirectionType::DT_Down;
		}
		return  EDirectionType::DT_Up;
	}

	return EDirectionType::DT_None;
}

FVector UCancerUIFunctionLibrary::GetInputActionValue(UObject* WorldContextObject, UInputAction* InputAction)
{
	const ULocalPlayer* LocalPlayer = WorldContextObject->GetWorld()->GetFirstPlayerController()->GetLocalPlayer();
	if (const UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		const UEnhancedPlayerInput* PlayerInput = Subsystem->GetPlayerInput();
		FInputActionValue Value = PlayerInput->GetActionValue(InputAction);
		return Value.Get<FVector>();
	}

	return FVector::ZeroVector;
}
