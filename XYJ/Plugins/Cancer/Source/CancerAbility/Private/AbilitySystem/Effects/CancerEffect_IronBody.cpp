#include "AbilitySystem/Effects/CancerEffect_IronBody.h"
#include "CancerNativeGamePlayTag.h"
#include "AbilitySystem/Attributes/CancerCombatSet.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "GameplayEffectComponents/AssetTagsGameplayEffectComponent.h"
UCancerEffect_IronBody::UCancerEffect_IronBody()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	{
		FSetByCallerFloat SetByCaller;
		SetByCaller.DataTag = SetByCaller_IronBody;

		FGameplayModifierInfo Mod;
		Mod.Attribute = UCancerCombatSet::GetIronBodyAttribute();
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		Modifiers.Add(Mod);
	}
}
