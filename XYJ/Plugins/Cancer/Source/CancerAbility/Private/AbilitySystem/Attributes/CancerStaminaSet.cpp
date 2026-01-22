#include "AbilitySystem/Attributes/CancerStaminaSet.h"

#include "CancerAbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

UCancerStaminaSet::UCancerStaminaSet()
{
}

void UCancerStaminaSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UCancerStaminaSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UCancerStaminaSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxStaminaAttribute())
	{
		// 确保当前耐力不超过新上限
		if (GetStamina() > NewValue)
		{
			if (UCancerAbilitySystemComponent* CancerASC = GetCancerAbilitySystemComponent())
			{
				CancerASC->ApplyModToAttribute(GetStaminaAttribute(), EGameplayModOp::Override, NewValue);
			}
		}

		OnMaxStaminaChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);
	}
	else if (Attribute == GetStaminaAttribute())
	{
		OnStaminaChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);

		const bool bWasOut = bOutOfStamina;
		bOutOfStamina = (GetStamina() <= 0.f);
		if (!bWasOut && bOutOfStamina)
		{
			OnOutOfStamina.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);
		}
		else if (bWasOut && !bOutOfStamina)
		{
			// 恢复有耐力
		}
	}
}

void UCancerStaminaSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
}
