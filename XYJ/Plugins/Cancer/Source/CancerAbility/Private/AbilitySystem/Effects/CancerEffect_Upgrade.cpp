#include "AbilitySystem/Effects/CancerEffect_Upgrade.h"
#include "AbilitySystem/Attributes/CancerHealthSet.h"
#include "AbilitySystem/Attributes/CancerSoulSet.h"
#include "AbilitySystem/Attributes/CancerStaminaSet.h"
#include "AbilitySystem/Attributes/CancerEnergySet.h"
#include "AbilitySystem/Attributes/CancerCombatSet.h"
#include "Engine/CurveTable.h"

UCancerEffect_Upgrade::UCancerEffect_Upgrade(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
}

void UCancerEffect_Upgrade::PostLoad()
{
	Super::PostLoad();
	BuildUpgradeModifiers();
}

void UCancerEffect_Upgrade::BuildUpgradeModifiers()
{
	Modifiers.Reset();
	{
		FAttributeBasedFloat Magnitude;
		Magnitude.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UCancerHealthSet::GetMaxHealthAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			/*bSnapshot*/ false);
		if (CurveTable)
		{
			Magnitude.Coefficient.Curve.CurveTable = CurveTable;
			Magnitude.Coefficient.Curve.RowName = CurveRowName;
		}
		else
		{
			Magnitude.Coefficient.Value = UpgradeCoeff;
		}
		FGameplayModifierInfo Info;
		Info.Attribute = UCancerHealthSet::GetMaxHealthAttribute();
		Info.ModifierOp = EGameplayModOp::Additive;
		Info.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
		Modifiers.Add(Info);
	}

	{
		FAttributeBasedFloat M;
		M.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UCancerSoulSet::GetMaxSoulAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		if (CurveTable)
		{
			M.Coefficient.Curve.CurveTable = CurveTable;
			M.Coefficient.Curve.RowName = CurveRowName_MaxSoul;
		}
		else
		{
			M.Coefficient.Value = UpgradeCoeff;
		}
		FGameplayModifierInfo I;
		I.Attribute = UCancerSoulSet::GetMaxSoulAttribute();
		I.ModifierOp = EGameplayModOp::Additive;
		I.ModifierMagnitude = FGameplayEffectModifierMagnitude(M);
		Modifiers.Add(I);
	}

	{
		FAttributeBasedFloat M;
		M.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UCancerStaminaSet::GetMaxStaminaAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		if (CurveTable)
		{
			M.Coefficient.Curve.CurveTable = CurveTable;
			M.Coefficient.Curve.RowName = CurveRowName_MaxStamina;
		}
		else
		{
			M.Coefficient.Value = UpgradeCoeff;
		}
		FGameplayModifierInfo I;
		I.Attribute = UCancerStaminaSet::GetMaxStaminaAttribute();
		I.ModifierOp = EGameplayModOp::Additive;
		I.ModifierMagnitude = FGameplayEffectModifierMagnitude(M);
		Modifiers.Add(I);
	}

	{
		FAttributeBasedFloat M;
		M.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UCancerEnergySet::GetMaxEnergyAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		if (CurveTable)
		{
			M.Coefficient.Curve.CurveTable = CurveTable;
			M.Coefficient.Curve.RowName = CurveRowName_MaxEnergy;
		}
		else
		{
			M.Coefficient.Value = UpgradeCoeff;
		}
		FGameplayModifierInfo I;
		I.Attribute = UCancerEnergySet::GetMaxEnergyAttribute();
		I.ModifierOp = EGameplayModOp::Additive;
		I.ModifierMagnitude = FGameplayEffectModifierMagnitude(M);
		Modifiers.Add(I);
	}

	{
		FAttributeBasedFloat M;
		M.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UCancerCombatSet::GetAttackPowerAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		if (CurveTable)
		{
			M.Coefficient.Curve.CurveTable = CurveTable;
			M.Coefficient.Curve.RowName = CurveRowName_AttackPower;
		}
		else
		{
			M.Coefficient.Value = UpgradeCoeff;
		}
		FGameplayModifierInfo I;
		I.Attribute = UCancerCombatSet::GetAttackPowerAttribute();
		I.ModifierOp = EGameplayModOp::Additive;
		I.ModifierMagnitude = FGameplayEffectModifierMagnitude(M);
		Modifiers.Add(I);
	}

	{
		FAttributeBasedFloat M;
		M.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UCancerCombatSet::GetPhysicalAttackPowerAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		if (CurveTable)
		{
			M.Coefficient.Curve.CurveTable = CurveTable;
			M.Coefficient.Curve.RowName = CurveRowName_PhysicalAttackPower;
		}
		else
		{
			M.Coefficient.Value = UpgradeCoeff;
		}
		FGameplayModifierInfo I;
		I.Attribute = UCancerCombatSet::GetPhysicalAttackPowerAttribute();
		I.ModifierOp = EGameplayModOp::Additive;
		I.ModifierMagnitude = FGameplayEffectModifierMagnitude(M);
		Modifiers.Add(I);
	}

	{
		FAttributeBasedFloat M;
		M.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UCancerCombatSet::GetDefenseAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		if (CurveTable)
		{
			M.Coefficient.Curve.CurveTable = CurveTable;
			M.Coefficient.Curve.RowName = CurveRowName_Defense;
		}
		else
		{
			M.Coefficient.Value = UpgradeCoeff;
		}
		FGameplayModifierInfo I;
		I.Attribute = UCancerCombatSet::GetDefenseAttribute();
		I.ModifierOp = EGameplayModOp::Additive;
		I.ModifierMagnitude = FGameplayEffectModifierMagnitude(M);
		Modifiers.Add(I);
	}

	{
		FAttributeBasedFloat M;
		M.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UCancerCombatSet::GetDamageBonusPctAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		if (CurveTable)
		{
			M.Coefficient.Curve.CurveTable = CurveTable;
			M.Coefficient.Curve.RowName = CurveRowName_DamageBonusPct;
		}
		else
		{
			M.Coefficient.Value = UpgradeCoeff;
		}
		FGameplayModifierInfo I;
		I.Attribute = UCancerCombatSet::GetDamageBonusPctAttribute();
		I.ModifierOp = EGameplayModOp::Additive;
		I.ModifierMagnitude = FGameplayEffectModifierMagnitude(M);
		Modifiers.Add(I);
	}

	{
		FAttributeBasedFloat M;
		M.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UCancerCombatSet::GetDamageReductionPctAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		if (CurveTable)
		{
			M.Coefficient.Curve.CurveTable = CurveTable;
			M.Coefficient.Curve.RowName = CurveRowName_DamageReductionPct;
		}
		else
		{
			M.Coefficient.Value = UpgradeCoeff;
		}
		FGameplayModifierInfo I;
		I.Attribute = UCancerCombatSet::GetDamageReductionPctAttribute();
		I.ModifierOp = EGameplayModOp::Additive;
		I.ModifierMagnitude = FGameplayEffectModifierMagnitude(M);
		Modifiers.Add(I);
	}

	{
		FAttributeBasedFloat M;
		M.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UCancerCombatSet::GetImpactAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		if (CurveTable)
		{
			M.Coefficient.Curve.CurveTable = CurveTable;
			M.Coefficient.Curve.RowName = CurveRowName_Impact;
		}
		else
		{
			M.Coefficient.Value = UpgradeCoeff;
		}
		FGameplayModifierInfo I;
		I.Attribute = UCancerCombatSet::GetImpactAttribute();
		I.ModifierOp = EGameplayModOp::Additive;
		I.ModifierMagnitude = FGameplayEffectModifierMagnitude(M);
		Modifiers.Add(I);
	}

	{
		FAttributeBasedFloat M;
		M.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
			UCancerCombatSet::GetIronBodyAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		if (CurveTable)
		{
			M.Coefficient.Curve.CurveTable = CurveTable;
			M.Coefficient.Curve.RowName = CurveRowName_IronBody;
		}
		else
		{
			M.Coefficient.Value = UpgradeCoeff;
		}
		FGameplayModifierInfo I;
		I.Attribute = UCancerCombatSet::GetIronBodyAttribute();
		I.ModifierOp = EGameplayModOp::Additive;
		I.ModifierMagnitude = FGameplayEffectModifierMagnitude(M);
		Modifiers.Add(I);
	}
}
