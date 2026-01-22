
#include "AbilitySystem/Cost/CancerAbilityCost_GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystem/CancerGameplayAbility.h"

bool UCancerAbilityCost_GameplayEffect::CheckCost(const UCancerGameplayAbility* Ability,
                                                  const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Ability)
		return false;
	auto ASC = Ability->GetCancerAbilitySystemComponentFromActorInfo();
	if (!ASC)
		return false;
	UGameplayEffect* CostGE = nullptr;
	if (CostGameplayEffectClass)
	{
		CostGE = CostGameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	}
	if (CostGE)
	{
		UAbilitySystemComponent* AbilitySystemComponent = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
		if (ensure(AbilitySystemComponent))
		{
			if (!AbilitySystemComponent->CanApplyAttributeModifiers(CostGE, Ability->GetAbilityLevel(Handle, ActorInfo),
			                                                        Ability->MakeEffectContext(Handle, ActorInfo)))
			{
				const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;

				if (OptionalRelevantTags && CostTag.IsValid())
				{
					OptionalRelevantTags->AddTag(CostTag);
				}
				return false;
			}
		}
	}

	auto CheckBaseValue = [](ECancerCostCheckType CheckType, float
	                Value, float TargetValue) -> bool
	{
		switch (CheckType)
		{
		case ECancerCostCheckType::Equal:
			return Value == TargetValue;
		case ECancerCostCheckType::Greater:
			return Value > TargetValue;
		case ECancerCostCheckType::GreaterOrEqual:
			return Value >= TargetValue;
		case ECancerCostCheckType::Less:
			return Value < TargetValue;
		case ECancerCostCheckType::LessOrEqual:
			return Value <= TargetValue;
		case ECancerCostCheckType::NotEqual:
			return Value != TargetValue;
		default: return true;
		}
	};

	//执行自定义的检测
	for (auto CostCheck : CostChecks)
	{
		if (CostCheck.MustTarget)
		{
			const float Value = ASC->GetNumericAttribute(CostCheck.CheckAttribute);
			const float TargetValue = ASC->GetNumericAttribute(CostCheck.CheckAttributeBase);
			float NewValue = Value / TargetValue;
			if (CheckBaseValue(CostCheck.CostCheckType, NewValue, CostCheck.PercentValue))
			{
				continue;
			}
			else
			{
				return false;
			}
		}
		else
		{
			const float Value = ASC->GetNumericAttribute(CostCheck.CheckAttribute);
			if (CheckBaseValue(CostCheck.CostCheckType, Value, CostCheck.Value))
			{
				continue;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

void UCancerAbilityCost_GameplayEffect::ApplyCost(const UCancerGameplayAbility* Ability,
                                                  const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!Ability)
		return;

	auto ASC = Ability->GetCancerAbilitySystemComponentFromActorInfo();
	if (!ASC)
		return;

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CostGameplayEffectClass, CostLevel,
	                                                             ASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();


		ASC->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}
