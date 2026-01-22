#include "AbilitySystem/Executions/CancerExecution_Soul.h"

#include "AbilitySystem/Attributes/CancerSoulSet.h"
#include "GameFramework/CancerDamageType.h"
#include "CancerNativeGamePlayTag.h"

UCancerExecution_Soul::UCancerExecution_Soul()
{
	
}

void UCancerExecution_Soul::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                   FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;
	
    const float SoulPerTick = Spec.GetSetByCallerMagnitude(SetByCaller_Soul, false, 0.0f);

    const UAbilitySystemComponent* TargetASCConst = ExecutionParams.GetTargetAbilitySystemComponent();
    UAbilitySystemComponent* TargetASC = const_cast<UAbilitySystemComponent*>(TargetASCConst);
    const float CurrSoul = TargetASC ? TargetASC->GetNumericAttribute(UCancerSoulSet::GetSoulAttribute()) : 0.0f;
    const float MaxSoul = TargetASC ? TargetASC->GetNumericAttribute(UCancerSoulSet::GetMaxSoulAttribute()) : 0.0f;
    const float Desired = CurrSoul + SoulPerTick;
    const float AddDelta = FMath::Clamp(Desired, 0.0f, MaxSoul) - CurrSoul;
    const float FinalSoul = FMath::Max(0.f, AddDelta);

    if (FinalSoul > 0.f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
            UCancerSoulSet::GetSoulAttribute(), EGameplayModOp::Additive, FinalSoul));
    }

    // 达到最大值后移除效果
    if (TargetASC && (CurrSoul + SoulPerTick) >= MaxSoul && MaxSoul > 0.0f)
    {
        FGameplayTagContainer Tags;
        Tags.AddTag(Tag_Combat_State_RestoreSoul);
        FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(Tags);
        TargetASC->RemoveActiveEffects(Query);
    }
}
