
#include "Action_AddLockingPointComponent.h"

#include "Components/CancerLockingPointComponent.h"

void UAction_AddLockingPointComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	Super::GatherSoftReferences(OutPaths);
}

void UAction_AddLockingPointComponent::PostInitComponent_Implementation()
{
	UCancerLockingPointComponent* LockingPointComponent = Cast<UCancerLockingPointComponent>(GetCreateComponent());
	check(LockingPointComponent);
	LockingPointComponent->IsOpen = IsOpen;
	LockingPointComponent->MaxLockingDistance = MaxLockingDistance;
	LockingPointComponent->MaxUnLockingDistance = MaxUnLockingDistance;
}
