#include "AbilitySystem/Attributes/CancerSoulSet.h"

#include "CancerAbilitySystemComponent.h"
#include "CancerNativeGamePlayTag.h"
#include "GameplayEffectExtension.h"

UCancerSoulSet::UCancerSoulSet()
{
}

void UCancerSoulSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UCancerSoulSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UCancerSoulSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxSoulAttribute())
	{
		if (GetSoul() > NewValue)
		{
			UCancerAbilitySystemComponent* CancerASC = GetCancerAbilitySystemComponent();
			check(CancerASC);

			CancerASC->ApplyModToAttribute(GetSoulAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
	if (bOutOfSoul&&GetSoul()>0.f)
	{
		bOutOfSoul = false;
	}
}

void UCancerSoulSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetMaxSoulAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetSoulAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxSoul());
	}
}

bool UCancerSoulSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
    if (!Super::PreGameplayEffectExecute(Data))
    {
        return false;
    }
	//记录变化前的值
	SoulBeforeAttributeChange = GetSoul();
	MaxSoulBeforeAttributeChange = GetMaxSoul();
    return true;
}

void UCancerSoulSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetSoulAttribute())
	{
		if (GetSoul() < SoulBeforeAttributeChange)
		{
			if (UCancerAbilitySystemComponent* CancerASC = GetCancerAbilitySystemComponent())
			{
				if (CancerASC->HasTag(Tag_Combat_State_UnlimitedSoul, true))
				{
					SetSoul(SoulBeforeAttributeChange);
				}
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMaxSoulAttribute())
	{
		OnMaxSoulChanged.Broadcast(Instigator,Causer,&Data.EffectSpec,Data.EvaluatedData.Magnitude,
			MaxSoulBeforeAttributeChange,GetMaxSoul());
	}

	if (GetSoul() != SoulBeforeAttributeChange)
	{
		OnSoulChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		                        SoulBeforeAttributeChange, GetSoul());
	}
	if (GetSoul() <= 0 && !bOutOfSoul)
	{
		OnOutOfSoul.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		                      SoulBeforeAttributeChange, GetSoul());
	}
	bOutOfSoul = GetSoul() <= 0;
}
