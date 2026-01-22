#include "AbilitySystem/Executions/CancerExecution_Damage.h"
#include "CancerNativeGamePlayTag.h"
#include "AbilitySystem/Attributes/CancerCombatSet.h"
#include "AbilitySystem/Attributes/CancerHealthSet.h"
#include "AbilitySystem/Attributes/CancerSoulSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Effects/CancerEffect_ApplySoul.h"
#include "GameFramework/CancerDamageType.h"


UE_DISABLE_OPTIMIZATION

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition AttackPowerDef;
	FGameplayEffectAttributeCaptureDefinition PhysicalAttackPowerDef;
	FGameplayEffectAttributeCaptureDefinition DamageBonusPctDef;
	FGameplayEffectAttributeCaptureDefinition DefenseDef;
	FGameplayEffectAttributeCaptureDefinition DamageReductionPctDef;


	FDamageStatics()
	{
		AttackPowerDef = FGameplayEffectAttributeCaptureDefinition(UCancerCombatSet::GetAttackPowerAttribute(),
		                                                           EGameplayEffectAttributeCaptureSource::Source, true);
		PhysicalAttackPowerDef = FGameplayEffectAttributeCaptureDefinition(
			UCancerCombatSet::GetPhysicalAttackPowerAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, true);

		DamageBonusPctDef = FGameplayEffectAttributeCaptureDefinition(UCancerCombatSet::GetDamageBonusPctAttribute(),
		                                                              EGameplayEffectAttributeCaptureSource::Source,
		                                                              true);
		DefenseDef = FGameplayEffectAttributeCaptureDefinition(UCancerCombatSet::GetDefenseAttribute(),
		                                                       EGameplayEffectAttributeCaptureSource::Target, true);
		DamageReductionPctDef = FGameplayEffectAttributeCaptureDefinition(
			UCancerCombatSet::GetDamageReductionPctAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UCancerExecution_Damage::UCancerExecution_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalAttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageBonusPctDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageReductionPctDef);
}

void UCancerExecution_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                     FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	FCancerDamageExecutionParameters Params;
	if (SourceTags) Params.SourceTags = *SourceTags;
	if (TargetTags) Params.TargetTags = *TargetTags;
	Params.SourceActor = Spec.GetEffectContext().GetInstigator();
	Params.TargetActor = ExecutionParams.GetTargetAbilitySystemComponent()
		                     ? ExecutionParams.GetTargetAbilitySystemComponent()->GetAvatarActor()
		                     : nullptr;
	Params.DamageType = Cast<UCancerDamageType>(Spec.GetEffectContext().GetSourceObject());

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvalParams,
	                                                           Params.AttackPower);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalAttackPowerDef, EvalParams,
	                                                           Params.PhysicalAttackPower);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageBonusPctDef, EvalParams,
	                                                           Params.DamageBonusPct);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvalParams, Params.Defense);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageReductionPctDef, EvalParams,
	                                                           Params.DamageReductionPct);


	//传入的伤害系数
	Params.DamageCoeff = Spec.GetSetByCallerMagnitude(SetByCaller_DamageCoeff, false, 1.0f);
	Params.SoulDamageCoeff = Spec.GetSetByCallerMagnitude(SetByCaller_SoulDamageCoeff, false, 1.0f);

	//传入的回复系数
	Params.DamageCoeff_X = Spec.GetSetByCallerMagnitude(SetByCaller_DamageCoeff_X, false,
	                                                    1.0f);
	Params.DamageCoeff_Y = Spec.GetSetByCallerMagnitude(SetByCaller_DamageCoeff_Y, false,
	                                                    1.0f);
	Params.DamageCoeff_Z = Spec.GetSetByCallerMagnitude(SetByCaller_DamageCoeff_Z, false,
	                                                    1.0f);

	FCancerDamageExecutionOutput Output;

	CalculateDamage(Params, Output);


	if (Output.FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UCancerHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, Output.FinalDamage));
	}

	if (Output.FinalSoulDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UCancerSoulSet::GetSoulAttribute(), EGameplayModOp::Additive, -Output.FinalSoulDamage));
	}
	if (Output.RecoveredSoul > 0.f)
	{
		if (UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
			Params.SourceActor))
		{
			FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
				UCancerEffect_ApplySoul::StaticClass(),
				1.0f, Context);
			if (FGameplayEffectSpec* SoulSpec = SpecHandle.Data.Get())
			{
				SoulSpec->SetSetByCallerMagnitude(SetByCaller_Soul, Output.RecoveredSoul);
				SourceASC->ApplyGameplayEffectSpecToSelf(*SoulSpec);
			}
		}
	}
}

void UCancerExecution_Damage::CalculateDamage(const FCancerDamageExecutionParameters& Params,
                                              FCancerDamageExecutionOutput& OutOutput) const
{
	float D = 0.f;
	float SD = 0.f;
	float SR = 0.f;
	K2_CalculateBaseDamage(Params, D);
	K2_CalculateSoulDamage(Params, SD);
	K2_CalculateSoulRecovery(Params, D, SD, SR);
	OutOutput.FinalDamage = D;
	OutOutput.FinalSoulDamage = SD;
	OutOutput.RecoveredSoul = SR;
}


void UCancerExecution_Damage::K2_CalculateBaseDamage_Implementation(const FCancerDamageExecutionParameters& Params,
                                                                    float& OutDamage) const
{
	float RawDamage = FMath::Max(0.f, Params.AttackPower);
	RawDamage *= (1.f + FMath::Max(0.f, Params.DamageBonusPct));
	RawDamage *= (1.f - FMath::Clamp(Params.DamageReductionPct, 0.f, 0.95f));
	float DamageCoeff = FMath::Max(0.f, Params.DamageCoeff);

	RawDamage -= Params.Defense;
	RawDamage *= DamageCoeff;

	OutDamage = FMath::Max(0.f, RawDamage);
}

void UCancerExecution_Damage::K2_CalculateSoulDamage_Implementation(const FCancerDamageExecutionParameters& Params,
                                                                    float& OutSoulDamage) const
{
	float PhysicalDamage = FMath::Max(0.f, Params.PhysicalAttackPower);
	PhysicalDamage *= Params.SoulDamageCoeff;
	const UAbilitySystemComponent* TargetASCConst = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		Params.TargetActor);
	const bool bRestoreSoul = (TargetASCConst && TargetASCConst->HasMatchingGameplayTag(Tag_Combat_State_RestoreSoul))
		|| (TargetASCConst && TargetASCConst->HasMatchingGameplayTag(Tag_Combat_State_InvulnerableSoul));
	if (bRestoreSoul) { PhysicalDamage = 0.f; }
	OutSoulDamage = PhysicalDamage;
}

void UCancerExecution_Damage::K2_CalculateSoulRecovery_Implementation(const FCancerDamageExecutionParameters& Params,
                                                                      const float RawDamage, const float PhysicalDamage,
                                                                      float& OutRecoveredSoul) const
{
	float RowSoul = RawDamage * Params.DamageCoeff_X + PhysicalDamage * Params.DamageCoeff_Y + Params.DamageCoeff_Z;
	OutRecoveredSoul = RowSoul;
}


UE_ENABLE_OPTIMIZATION
