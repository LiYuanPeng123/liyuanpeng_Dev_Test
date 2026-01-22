
#include "Providers/Animation/AnimationProvider_HitReaction.h"

#include "AbilitySystem/CancerGameplayAbility.h"

UAnimMontage* UAnimationProvider_HitReaction::GetMontageToPlay_Implementation(
	UCancerGameplayAbility* CombatAbility) const
{
	return Super::GetMontageToPlay_Implementation(CombatAbility);
}

bool UAnimationProvider_HitReaction::CalculateAnimationAngle_Implementation(const UCancerGameplayAbility* CombatAbility,
	float& OutAngle) const
{
	const AActor* Avatar = CombatAbility->GetAvatarActorFromActorInfo();
	if (Avatar == nullptr)
	{
		OutAngle = 0.f;
		return false;
	}

	static constexpr float FrontAngle = 0.f;

	return false;
}
