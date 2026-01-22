
#include "CancerLockingSubsystem.h"

void UCancerLockingSubsystem::AddLockingPoint(UCancerLockingPointComponent* LockingPoint)
{
	LockingPointArray.Add(LockingPoint);
}

void UCancerLockingSubsystem::RemoveLockingPoint(UCancerLockingPointComponent* LockingPoint)
{
	LockingPointArray.Remove(LockingPoint);
}

void UCancerLockingSubsystem::ClearLockingPoints()
{
	LockingPointArray.Empty();
}

TArray<class UCancerLockingPointComponent*> UCancerLockingSubsystem::GetAllLockingPointArray()
{
	return LockingPointArray;
}
