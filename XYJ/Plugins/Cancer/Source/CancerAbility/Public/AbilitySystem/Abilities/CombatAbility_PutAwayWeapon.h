
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CombatAbility_PutAwayWeapon.generated.h"

/**
 * 收刀
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_PutAwayWeapon : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()
	public:
	
	UCombatAbility_PutAwayWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
