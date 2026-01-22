#include "AbilitySystem/Attributes/CancerEnergySet.h"
#include "CancerAbilitySystemComponent.h"
#include "CancerNativeGamePlayTag.h"
#include "GameplayEffectExtension.h"

UCancerEnergySet::UCancerEnergySet()
{
}

void UCancerEnergySet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UCancerEnergySet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UCancerEnergySet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    if (Attribute == GetMaxEnergyAttribute())
    {
        if (GetEnergy() > NewValue)
        {
            if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponent())
            {
                ASC->ApplyModToAttribute(GetEnergyAttribute(), EGameplayModOp::Override, NewValue);
            }
        }
        OnMaxEnergyChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);
    }
    else if (Attribute == GetEnergyAttribute())
    {
        OnEnergyChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);

        const bool bWasOut = bOutOfMana;
        bOutOfMana = (GetEnergy() <= 0.f);
        if (!bWasOut && bOutOfMana)
        {
            OnOutOfEnergy.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);
        }
    }
}

void UCancerEnergySet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetEnergyAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxEnergy());
    }
    else if (Attribute == GetMaxEnergyAttribute())
    {
        NewValue = FMath::Max(NewValue, 1.f);
    }
}

bool UCancerEnergySet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	//记录变化前的值
	EnergyBeforeAttributeChange = GetEnergy();
	MaxEnergyBeforeAttributeChange = GetMaxEnergy();
	return true;
}

void UCancerEnergySet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetEnergyAttribute())
	{
		if (GetEnergy() < EnergyBeforeAttributeChange)
		{
			if (UCancerAbilitySystemComponent* CancerASC = GetCancerAbilitySystemComponent())
			{
				if (CancerASC->HasTag(Tag_Combat_State_UnlimitedEnergy, true))
				{
					SetEnergy(EnergyBeforeAttributeChange);
				}
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMaxEnergyAttribute())
	{
		OnMaxEnergyChanged.Broadcast(Instigator,Causer,&Data.EffectSpec,Data.EvaluatedData.Magnitude,
			MaxEnergyBeforeAttributeChange,GetMaxEnergy());
	}

	if (GetEnergy() != EnergyBeforeAttributeChange)
	{
		OnEnergyChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
								EnergyBeforeAttributeChange, GetEnergy());
	}
	if (GetEnergy() <= 0 && !bOutOfMana)
	{
		OnOutOfEnergy.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
							  EnergyBeforeAttributeChange, GetEnergy());
	}
	bOutOfMana = GetEnergy() <= 0;
}