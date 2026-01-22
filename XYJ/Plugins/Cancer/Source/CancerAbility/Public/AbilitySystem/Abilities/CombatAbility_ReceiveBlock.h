
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerCombatAnimationAbility.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CombatAbility_ReceiveBlock.generated.h"


/**
 * 格挡成功后 触发的被动格挡技能
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_ReceiveBlock : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()

public:
	UCombatAbility_ReceiveBlock();
	
};
