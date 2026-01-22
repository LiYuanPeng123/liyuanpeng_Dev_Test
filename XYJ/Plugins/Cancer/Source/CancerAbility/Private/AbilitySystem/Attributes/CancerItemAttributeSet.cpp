#include "AbilitySystem/Attributes/CancerItemAttributeSet.h"

UCancerItemAttributeSet::UCancerItemAttributeSet()
{
}

void UCancerItemAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UCancerItemAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UCancerItemAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UCancerItemAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetAttackPowerBonusAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.f);
    }
    else if (Attribute == GetDefenseBonusAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.f);
    }
    else if (Attribute == GetDamageBonusPctBonusAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, 5.f);
    }
    else if (Attribute == GetDamageReductionPctBonusAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, 0.95f);
    }
    else if (Attribute == GetWeaponLevelAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 1.f, 100.f);
    }
}