#include "AbilitySystem/Effects/CancerEffect_ApplySoul.h"
#include "CancerNativeGamePlayTag.h"
#include "AbilitySystem/Attributes/CancerSoulSet.h"
#include "GameplayEffectComponents/AssetTagsGameplayEffectComponent.h"
#include "GameplayEffectComponents/BlockAbilityTagsGameplayEffectComponent.h"


UCancerEffect_ApplySoul::UCancerEffect_ApplySoul()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	{
		FSetByCallerFloat SetByCaller;
		SetByCaller.DataTag = SetByCaller_Soul;
		FGameplayModifierInfo Mod;
		Mod.Attribute = UCancerSoulSet::GetSoulAttribute();
		Mod.ModifierOp = EGameplayModOp::Additive;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		Modifiers.Add(Mod);
	}
}
