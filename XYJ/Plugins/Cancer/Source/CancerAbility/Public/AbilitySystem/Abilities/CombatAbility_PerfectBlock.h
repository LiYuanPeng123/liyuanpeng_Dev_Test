
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CombatAbility_PerfectBlock.generated.h"

/**
 * 完美格挡 主动方
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_PerfectBlock : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()
	public:
	UCombatAbility_PerfectBlock();
	
};
