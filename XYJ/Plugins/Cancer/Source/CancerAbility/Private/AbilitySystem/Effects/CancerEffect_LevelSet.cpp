#include "AbilitySystem/Effects/CancerEffect_LevelSet.h"
#include "CancerNativeGamePlayTag.h"
#include "AbilitySystem/Attributes/CancerProgressionSet.h"

UCancerEffect_LevelSet::UCancerEffect_LevelSet()
{
	{
		FSetByCallerFloat SetByCaller;
		SetByCaller.DataTag = SetByCaller_Cure;

		FGameplayModifierInfo Mod;
		Mod.Attribute = UCancerProgressionSet::GetLevelAttribute();
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		Modifiers.Add(Mod);
	}
	{
		FSetByCallerFloat SetByCaller;
		SetByCaller.DataTag = SetByCaller_Cure;

		FGameplayModifierInfo Mod;
		Mod.Attribute = UCancerProgressionSet::GetExpAttribute();
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		Modifiers.Add(Mod);
	}
}
