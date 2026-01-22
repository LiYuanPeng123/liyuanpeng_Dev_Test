
#include "AbilitySystem/Cost/CancerAbilityCost_ApplyGE.h"

#include "AbilitySystemComponent.h"
#include "Misc/DataValidation.h"

bool UCancerAbilityCost_ApplyGE::CheckCost(const UCancerGameplayAbility* Ability,
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Ability || !ActorInfo)
	{
		return false;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC || !CheckAttribute.IsValid())
	{
		return false;
	}

	const float CurrentValue = ASC->GetNumericAttribute(CheckAttribute);
	float Threshold = CheckValue.GetValueAtLevel(Ability->GetAbilityLevel());

	// 如果配置了基准属性，Threshold变为系数
	if (ReferenceAttribute.IsValid())
	{
		const float RefValue = ASC->GetNumericAttribute(ReferenceAttribute);
		
		// 基准值为0时直接失败，防止除0错误或逻辑漏洞
		if (FMath::IsNearlyZero(RefValue))
		{
			return false;
		}
		
		Threshold *= RefValue;
	}

	switch (CheckType)
	{
	case ECheckerType::Equal:
		return FMath::IsNearlyEqual(CurrentValue, Threshold);

	case ECheckerType::NotEqual:
		return !FMath::IsNearlyEqual(CurrentValue, Threshold);

	case ECheckerType::Greater:
		return CurrentValue > Threshold;

	case ECheckerType::GreaterOrEqual:
		return CurrentValue >= Threshold;

	case ECheckerType::Less:
		return CurrentValue < Threshold;

	case ECheckerType::LessOrEqual:
		return CurrentValue <= Threshold;
	}

	return false;
}

void UCancerAbilityCost_ApplyGE::ApplyCost(const UCancerGameplayAbility* Ability,
                                           const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!Ability || !ActorInfo || !ApplyGE)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = Ability->MakeOutgoingGameplayEffectSpec(ApplyGE, Ability->GetAbilityLevel());
	if (SpecHandle.IsValid())
	{
		if (InjectionTag.IsValid())
		{
			const float AbilityLevel = Ability->GetAbilityLevel();
			float Magnitude = CheckValue.GetValueAtLevel(AbilityLevel);

			// 如果CostValue被显式配置过（值不为0或引用了曲线），则优先使用CostValue
			if (!FMath::IsNearlyZero(CostValue.Value) || !CostValue.Curve.RowName.IsNone())
			{
				Magnitude = CostValue.GetValueAtLevel(AbilityLevel);
			}

			// 如果配置了基准属性，注入值也进行缩放
			if (ReferenceAttribute.IsValid())
			{
				const float RefValue = ASC->GetNumericAttribute(ReferenceAttribute);
				Magnitude *= RefValue;
			}
			
			SpecHandle.Data->SetSetByCallerMagnitude(InjectionTag, Magnitude);
		}
		// 传递PredictionKey以保证预测链完整
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get(), ActivationInfo.GetActivationPredictionKey());
	}
}

#if WITH_EDITOR
EDataValidationResult UCancerAbilityCost_ApplyGE::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!ApplyGE)
	{
		Context.AddError(FText::FromString(TEXT("ApplyGE is not configured! Cost will verify check but apply nothing.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!CheckAttribute.IsValid())
	{
		Context.AddError(FText::FromString(TEXT("CheckAttribute is invalid!")));
		Result = EDataValidationResult::Invalid;
	}

	if (InjectionTag.IsValid() && !ApplyGE)
	{
		Context.AddError(FText::FromString(TEXT("InjectionTag is set but ApplyGE is missing!")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

FString UCancerAbilityCost_ApplyGE::GetDescription(int32 Level) const
{
	const float Val = CheckValue.GetValueAtLevel(Level);
	FString AttributeName = CheckAttribute.GetName();
	
	if (ReferenceAttribute.IsValid())
	{
		return FString::Printf(TEXT("Requires %s %.0f%% of %s"), *AttributeName, Val * 100.f, *ReferenceAttribute.GetName());
	}
	
	return FString::Printf(TEXT("Requires %s %.0f"), *AttributeName, Val);
}
