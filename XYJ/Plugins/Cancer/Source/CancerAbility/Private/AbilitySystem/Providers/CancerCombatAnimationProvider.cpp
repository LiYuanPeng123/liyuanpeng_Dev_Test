

#include "Providers/CancerCombatAnimationProvider.h"

#include "CancerAbilitySystemComponent.h"
#include "AbilitySystem/CancerGameplayAbility.h"

UCancerCombatAnimationProvider::UCancerCombatAnimationProvider()
{
	DefaultAnimationMontage = nullptr;
	DefaultSectionName = NAME_None;
	DefaultPlayRate = 1.f;
	RootMotionScale = 1.f;
	MontageTag = FGameplayTag::EmptyTag;
}

UAnimMontage* UCancerCombatAnimationProvider::GetMontageToPlay_Implementation(
	UCancerGameplayAbility* CombatAbility) const
{
	if (CombatAbility)
	{
		if (MontageTag.IsValid())
			return CombatAbility->GetCancerAbilitySystemComponentFromActorInfo()->GetAbilityMontage(MontageTag);
		return DefaultAnimationMontage;
	}
	return nullptr;
}

FName UCancerCombatAnimationProvider::GetSectionName_Implementation(UCancerGameplayAbility* CombatAbility) const
{

	return DefaultSectionName;
}

float UCancerCombatAnimationProvider::GetPlayRate_Implementation(UCancerGameplayAbility* CombatAbility) const
{
	return DefaultPlayRate;
}

float UCancerCombatAnimationProvider::GetRootMotionScale_Implementation(UCancerGameplayAbility* CombatAbility) const
{
	return RootMotionScale;
}
