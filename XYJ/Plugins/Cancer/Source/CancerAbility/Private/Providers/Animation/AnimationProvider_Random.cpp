

#include "Providers/Animation/AnimationProvider_Random.h"

UAnimMontage* UAnimationProvider_Random::GetMontageToPlay_Implementation(UCancerGameplayAbility* CombatAbility) const
{
	const int32 AnimCount = RandomAnimations.Num(); 
	if (AnimCount == 1)
	{
		// We only have one item. No need to perform random number calculation.
		return RandomAnimations[0].AnimMontage;
	}

	const int32 Index = FMath::RandRange(0, AnimCount - 1);
	return RandomAnimations[Index].AnimMontage;
}
