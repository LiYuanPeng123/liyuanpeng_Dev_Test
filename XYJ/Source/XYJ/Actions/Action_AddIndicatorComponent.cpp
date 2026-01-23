

#include "Action_AddIndicatorComponent.h"

#include "CancerIndicatorManagerComponent.h"

UAction_AddIndicatorComponent::UAction_AddIndicatorComponent()
{
	CancerActionType = ECancerActionType::Controller;
}

void UAction_AddIndicatorComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	Super::GatherSoftReferences(OutPaths);
}

void UAction_AddIndicatorComponent::PostInitComponent_Implementation()
{
	Super::PostInitComponent_Implementation();
	UCancerIndicatorManagerComponent* Indicator = Cast<UCancerIndicatorManagerComponent>(GetCreateComponent());
	
}
