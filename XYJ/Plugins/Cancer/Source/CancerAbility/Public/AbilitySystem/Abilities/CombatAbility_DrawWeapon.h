
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CombatAbility_DrawWeapon.generated.h"

/**
 *  拔刀
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_DrawWeapon : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()
	public:
	UCombatAbility_DrawWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
