#include "AbilitySystem/Effects/CancerEffect_EquipItemBonus.h"
#include "AbilitySystem/Attributes/CancerCombatSet.h"

const FName UCancerEffect_EquipItemBonus::SetByCaller_AttackPowerBonus(TEXT("Equip.AttackPowerBonus"));
const FName UCancerEffect_EquipItemBonus::SetByCaller_DefenseBonus(TEXT("Equip.DefenseBonus"));
const FName UCancerEffect_EquipItemBonus::SetByCaller_DamageBonusPct(TEXT("Equip.DamageBonusPct"));
const FName UCancerEffect_EquipItemBonus::SetByCaller_DamageReductionPct(TEXT("Equip.DamageReductionPct"));

UCancerEffect_EquipItemBonus::UCancerEffect_EquipItemBonus()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	{
		FSetByCallerFloat SetByCaller;
		SetByCaller.DataName = SetByCaller_AttackPowerBonus;

		FGameplayModifierInfo Mod;
		Mod.Attribute = UCancerCombatSet::GetAttackPowerAttribute();
		Mod.ModifierOp = EGameplayModOp::Additive;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		Modifiers.Add(Mod);
	}
	{
		FSetByCallerFloat SetByCaller;
		SetByCaller.DataName = SetByCaller_DefenseBonus;

		FGameplayModifierInfo Mod;
		Mod.Attribute = UCancerCombatSet::GetDefenseAttribute();
		Mod.ModifierOp = EGameplayModOp::Additive;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		Modifiers.Add(Mod);
	}
	{
		FSetByCallerFloat SetByCaller;
		SetByCaller.DataName = SetByCaller_DamageBonusPct;

		FGameplayModifierInfo Mod;
		Mod.Attribute = UCancerCombatSet::GetDamageBonusPctAttribute();
		Mod.ModifierOp = EGameplayModOp::Additive;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		Modifiers.Add(Mod);
	}
	{
		FSetByCallerFloat SetByCaller;
		SetByCaller.DataName = SetByCaller_DamageReductionPct;

		FGameplayModifierInfo Mod;
		Mod.Attribute = UCancerCombatSet::GetDamageReductionPctAttribute();
		Mod.ModifierOp = EGameplayModOp::Additive;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		Modifiers.Add(Mod);
	}
}
