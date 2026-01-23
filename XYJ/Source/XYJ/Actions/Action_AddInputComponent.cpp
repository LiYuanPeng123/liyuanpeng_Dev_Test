#include "Action_AddInputComponent.h"
#include "Components/CancerInputManagerComponent.h"


void UAction_AddInputComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	if (!InputConfig.IsNull())
		OutPaths.Add(InputConfig.ToSoftObjectPath());
}

void UAction_AddInputComponent::PreInitComponent_Implementation()
{
	UCancerInputManagerComponent* InputManager = Cast<UCancerInputManagerComponent>(GetCreateComponent());

	check(InputManager != nullptr);
}

void UAction_AddInputComponent::FinalizeAfterComponent_Implementation()
{
	Super::FinalizeAfterComponent_Implementation();
	
	UCancerInputManagerComponent* InputManager = Cast<UCancerInputManagerComponent>(GetCreateComponent());
	InputManager->AddInputSetupData(InputConfig.Get());
}
