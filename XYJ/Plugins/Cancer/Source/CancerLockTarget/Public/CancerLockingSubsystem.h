
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CancerLockingSubsystem.generated.h"


class UCancerLockingPointComponent;

UCLASS()
class CANCERLOCKTARGET_API UCancerLockingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void AddLockingPoint(UCancerLockingPointComponent* LockingPoint);
	void RemoveLockingPoint(UCancerLockingPointComponent* LockingPoint);
	void ClearLockingPoints();
	TArray<class UCancerLockingPointComponent*> GetAllLockingPointArray();
protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<class UCancerLockingPointComponent*> LockingPointArray;
};
