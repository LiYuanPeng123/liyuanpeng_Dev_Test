
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CancerCombatDelegates.generated.h"


DECLARE_DYNAMIC_DELEGATE(FCombatAnimationEventDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatAnimationEventMCDelegate);

UCLASS()
class CANCERABILITY_API UCancerCombatDelegates : public UObject
{
	GENERATED_BODY()
};
