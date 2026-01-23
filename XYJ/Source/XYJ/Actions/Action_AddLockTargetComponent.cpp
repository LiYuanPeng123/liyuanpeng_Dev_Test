#include "Action_AddLockTargetComponent.h"
#include "Components/CancerLockingTargetComponent.h"
#include "Components/WidgetComponent.h"

void UAction_AddLockTargetComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	if (!LockingWidgetComponentClass.IsNull())
		OutPaths.Add(LockingWidgetComponentClass.ToSoftObjectPath());
	if (!LockingPointWidgetClass.IsNull())
		OutPaths.Add(LockingPointWidgetClass.ToSoftObjectPath());
}

void UAction_AddLockTargetComponent::PostInitComponent_Implementation()
{
	UCancerLockingTargetComponent* LockingTargetComponent = Cast<UCancerLockingTargetComponent>(GetCreateComponent());
	check(LockingTargetComponent);

	LockingTargetComponent->MaxLockingDistance = MaxLockingDistance;
	LockingTargetComponent->LockingWidgetComponentClass = LockingWidgetComponentClass.Get();
	LockingTargetComponent->LockingPointWidgetClass = LockingPointWidgetClass.Get();
	LockingTargetComponent->LockContainer = LockContainer;
}
