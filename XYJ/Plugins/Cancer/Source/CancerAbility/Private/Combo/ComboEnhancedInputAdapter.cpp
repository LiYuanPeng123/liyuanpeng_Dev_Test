#include "Combo/ComboEnhancedInputAdapter.h"
#include "Combo/ComboEnhancedInputBindings.h"
#include "Combo/ComboInput.h"
#include "Combo/ComboInputConfig.h"
#include "Combo/ComboInputMapping.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"

UComboEnhancedInputAdapter::UComboEnhancedInputAdapter()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UComboEnhancedInputAdapter::BeginPlay()
{
	Super::BeginPlay();
	InitializeAdapter();
}

void UComboEnhancedInputAdapter::InitializeAdapter()
{
	if (!InputComponentRef)
	{
		InputComponentRef = GetOwner() ? GetOwner()->FindComponentByClass<UComboInputComponent>() : nullptr;
	}
	APlayerController* PC = GetOwner() ? Cast<APlayerController>(GetOwner()->GetInstigatorController()) : nullptr;
	if (!PC && GetOwner())
	{
		PC = Cast<APlayerController>(GetOwner()->GetOwner());
	}
	if (PC)
	{
		EnhancedInputComp = Cast<UEnhancedInputComponent>(PC->InputComponent);
		if (Bindings && Bindings->MappingContext)
		{
			if (ULocalPlayer* LP = PC->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsys = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					Subsys->AddMappingContext(Bindings->MappingContext, Bindings->MappingPriority);
				}
			}
		}
	}
	BindActions();
}

void UComboEnhancedInputAdapter::BindActions()
{
	if (!EnhancedInputComp || !Bindings) return;
	for (const FComboEnhancedBinding& B : Bindings->Bindings)
	{
		if (!B.Action) continue;
		EnhancedInputComp->BindAction(B.Action, ETriggerEvent::Started, this, &ThisClass::OnActionStarted);
		EnhancedInputComp->BindAction(B.Action, ETriggerEvent::Triggered, this, &ThisClass::OnActionTriggered);
		EnhancedInputComp->BindAction(B.Action, ETriggerEvent::Completed, this, &ThisClass::OnActionCompleted);
	}
}

void UComboEnhancedInputAdapter::OnActionStarted(const FInputActionInstance& Instance)
{
	if (!InputComponentRef || !Bindings) return;
	const UInputAction* Action = Instance.GetSourceAction();
	for (const FComboEnhancedBinding& B : Bindings->Bindings)
	{
		if (B.Action == Action)
		{
			FGameplayTagContainer Physical;
			if (B.PhysicalTag.IsValid())
			{
				Physical.AddTag(B.PhysicalTag);
			}
			InputComponentRef->BufferPressTags(Physical);
			if (B.bIsAxis && B.AxisTag.IsValid())
			{
				const float Value = Instance.GetValue().GetMagnitude();
				InputComponentRef->FeedAxis(B.AxisTag, Value);
			}
			break;
		}
	}
}

void UComboEnhancedInputAdapter::OnActionTriggered(const FInputActionInstance& Instance)
{
	if (!InputComponentRef || !Bindings) return;
	const UInputAction* Action = Instance.GetSourceAction();
	for (const FComboEnhancedBinding& B : Bindings->Bindings)
	{
		if (B.Action == Action && B.bIsAxis && B.AxisTag.IsValid())
		{
			const float Value = Instance.GetValue().GetMagnitude();
			InputComponentRef->FeedAxis(B.AxisTag, Value);
			break;
		}
	}
}

void UComboEnhancedInputAdapter::OnActionCompleted(const FInputActionInstance& Instance)
{
	if (!InputComponentRef || !Bindings) return;
	const UInputAction* Action = Instance.GetSourceAction();
	for (const FComboEnhancedBinding& B : Bindings->Bindings)
	{
		if (B.Action == Action)
		{
			FGameplayTagContainer Physical;
			if (B.PhysicalTag.IsValid())
			{
				Physical.AddTag(B.PhysicalTag);
			}
			InputComponentRef->BufferReleaseTags(Physical);
			break;
		}
	}
}

