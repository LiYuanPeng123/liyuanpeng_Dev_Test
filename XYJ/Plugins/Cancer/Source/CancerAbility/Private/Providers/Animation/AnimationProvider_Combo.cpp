#include "Providers/Animation/AnimationProvider_Combo.h"
#include "CancerAbilitySystemComponent.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "Components/CancerHeroComponent.h"

UE_DISABLE_OPTIMIZATION

UAnimMontage* UAnimationProvider_Combo::GetMontageToPlay_Implementation(UCancerGameplayAbility* CombatAbility) const
{
	if (auto HeroComponent = CombatAbility->GetHeroComponentFromActorInfo())
	{
		int32 ComboIndex = HeroComponent->ComboIndex;
		if (auto ASC = CombatAbility->GetCancerAbilitySystemComponentFromActorInfo())
		{
			if (bUseTag)
			{
				return ASC->GetAbilityMontage(CombatAnimMontages[ComboIndex].AnimMontageTag);
			}
			if (CombatAnimMontages.IsValidIndex(ComboIndex))
				return CombatAnimMontages[ComboIndex].AnimMontage;
		}
		return nullptr;
	}
	return nullptr;
}

UE_ENABLE_OPTIMIZATION
