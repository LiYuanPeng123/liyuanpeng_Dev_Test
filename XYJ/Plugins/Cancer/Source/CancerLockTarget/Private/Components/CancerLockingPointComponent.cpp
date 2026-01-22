

#include "Components/CancerLockingPointComponent.h"

#include "CancerLockingSubsystem.h"




void UCancerLockingPointComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		if (UCancerLockingSubsystem* LockingSubsystem = GetWorld()->GetSubsystem<UCancerLockingSubsystem>())
		{
			LockingSubsystem->AddLockingPoint(this);
		}
	}
	
}

void UCancerLockingPointComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (GetWorld())
	{
		if (UCancerLockingSubsystem* LockingSubsystem = GetWorld()->GetSubsystem<UCancerLockingSubsystem>())
		{
			LockingSubsystem->RemoveLockingPoint(this);
		}
	}
}




