#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatAttackAbilityInterface.generated.h"


class UAbilityTask_ScanMeleeTarget;
class UCancerMeleeScan;

UINTERFACE()
class UCombatAttackAbilityInterface : public UInterface
{
	GENERATED_BODY()
};


class CANCERABILITY_API ICombatAttackAbilityInterface
{
	GENERATED_BODY()

public:
	virtual UAbilityTask_ScanMeleeTarget* GetMeleeScanTask() const = 0;
	virtual void InitializeMeleeScanTask(const UCancerMeleeScan* MeleeScan) = 0;
};
