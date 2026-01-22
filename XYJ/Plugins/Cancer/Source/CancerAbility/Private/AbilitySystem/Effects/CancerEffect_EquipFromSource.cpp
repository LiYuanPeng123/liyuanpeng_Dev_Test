#include "AbilitySystem/Effects/CancerEffect_EquipFromSource.h"
#include "AbilitySystem/Attributes/CancerCombatSet.h"
#include "AbilitySystem/Attributes/CancerItemAttributeSet.h"

UCancerEffect_EquipFromSource::UCancerEffect_EquipFromSource()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	auto AddSourceCapture = [&](const FGameplayAttribute& SourceAttr, const FGameplayAttribute& TargetAttr)
	{
		FAttributeBasedFloat Magnitude;
		Magnitude.Coefficient = 1.f;
		Magnitude.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			SourceAttr,
			EGameplayEffectAttributeCaptureSource::Source,
			/*bSnapshot*/ false);

		FGameplayModifierInfo Mod;
		Mod.Attribute = TargetAttr;
		Mod.ModifierOp = EGameplayModOp::Additive;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
		Modifiers.Add(Mod);
	};

	// AttackPower += Source.AttackPowerBonus
	AddSourceCapture(UCancerItemAttributeSet::GetAttackPowerBonusAttribute(),
	                 UCancerCombatSet::GetAttackPowerAttribute());

	// Defense += Source.DefenseBonus
	AddSourceCapture(UCancerItemAttributeSet::GetDefenseBonusAttribute(),
	                 UCancerCombatSet::GetDefenseAttribute());

	// DamageBonusPct += Source.DamageBonusPctBonus
	AddSourceCapture(UCancerItemAttributeSet::GetDamageBonusPctBonusAttribute(),
	                 UCancerCombatSet::GetDamageBonusPctAttribute());

	// DamageReductionPct += Source.DamageReductionPctBonus
	AddSourceCapture(UCancerItemAttributeSet::GetDamageReductionPctBonusAttribute(),
	                 UCancerCombatSet::GetDamageReductionPctAttribute());
}
