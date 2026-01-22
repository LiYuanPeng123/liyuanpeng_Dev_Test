
#include "AbilitySystem/Calculation/CancerAbilityCalculation.h"

#include "CancerAbilityFunctionLibrary.h"
#include "CancerAbilitySystemComponent.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "AbilitySystem/Attributes/CancerProgressionSet.h"


float UCancerAbilityCalculation::BP_GetCapturedAttributeMagnitude(const FGameplayEffectSpec& EffectSpec, FGameplayAttribute Attribute) const
{
	float Magnitude = 0.0f;

	// look for the attribute in the capture list
	for (const FGameplayEffectAttributeCaptureDefinition& CurrentCapture : RelevantAttributesToCapture)
	{
		if (CurrentCapture.AttributeToCapture == Attribute)
		{
			// configure the aggregator evaluation parameters
			// TODO: apply filters?
			FAggregatorEvaluateParameters EvaluationParameters;

			EvaluationParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
			EvaluationParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

			// get the attribute magnitude
			GetCapturedAttributeMagnitude(CurrentCapture, EffectSpec, EvaluationParameters, Magnitude);

			// capture found. Stop the search
			break;
		}
	}

	return Magnitude;
}

float UCancerAbilityCalculation::BP_GetFloatMagnitudeByTag(const FGameplayEffectSpec& EffectSpec, const FGameplayTag Tag) const
{

	return EffectSpec.GetSetByCallerMagnitude(Tag);
}


bool UCancerAbilityCalculation::BP_HasTag(const FGameplayEffectSpec& EffectSpec, const FGameplayTag Tag) const
{
	return EffectSpec.GetDynamicAssetTags().HasTag(Tag);

}

float UCancerAbilityCalculation::BP_GetLevel(const FGameplayEffectSpec& EffectSpec) const
{
	return EffectSpec.GetLevel();
}

FCancerAbilityDef UCancerAbilityCalculation::BP_GetDefinition(const FGameplayEffectSpec& EffectSpec) const
{
	const UCancerGameplayAbility* Ability = Cast<UCancerGameplayAbility>(
		EffectSpec.GetContext().GetAbilityInstance_NotReplicated());

	if (Ability)
	{
		return Ability->GetAbilityData().AbilityDef;
	}
	return FCancerAbilityDef();
}

float UCancerAbilityCalculation::BP_GetCoolDown(const FGameplayEffectSpec& EffectSpec) const
{
	const UCancerGameplayAbility* Ability = Cast<UCancerGameplayAbility>(
		EffectSpec.GetContext().GetAbilityInstance_NotReplicated());

	if (Ability)
	{
		const FCancerAbilityDataSet& AbilityData = Ability->GetAbilityData();
		return AbilityData.AbilityDef.CooldownDuration.GetValueAtLevel(Ability->GetAbilityLevel());
	}
			
	return 0.0f;
}

float UCancerAbilityCalculation::BP_GetCost(const FGameplayEffectSpec& EffectSpec) const
{
	const UCancerGameplayAbility* Ability = Cast<UCancerGameplayAbility>(
		EffectSpec.GetContext().GetAbilityInstance_NotReplicated());

	if (Ability)
	{
		const FCancerAbilityDataSet& AbilityData = Ability->GetAbilityData();
		return AbilityData.AbilityDef.Cost.GetValueAtLevel(Ability->GetAbilityLevel());
	}
			
	return 0.0f;
}

AActor* UCancerAbilityCalculation::BP_GetInstigator(const FGameplayEffectSpec& EffectSpec) const
{
	return EffectSpec.GetContext().GetEffectCauser();
}

float UCancerAbilityCalculation::BP_GetAttributeSet(const FGameplayEffectSpec& EffectSpec,
                                                    const FGameplayTag& InTag) const
{
	if (auto ASC = UCancerAbilityFunctionLibrary::GetCancerAbilitySystemComponent(BP_GetInstigator
		(EffectSpec)))
	{
		if (auto Curve = ASC->GetAttributeSetData().AttributeMap.Find(InTag))
		{
			float CurrentLevel = ASC->GetLevel();
			return Curve->GetValueAtLevel(CurrentLevel);
		}
			
	}
	return 0.0f;
}
