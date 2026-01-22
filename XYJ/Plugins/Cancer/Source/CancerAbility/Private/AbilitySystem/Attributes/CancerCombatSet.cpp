#include "AbilitySystem/Attributes/CancerCombatSet.h"

#include "CancerAbilitySystemComponent.h"
#include "Statics/CancerStaticsSubsystem.h"

UCancerCombatSet::UCancerCombatSet()
{
}

void UCancerCombatSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UCancerCombatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UCancerCombatSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UCancerCombatSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetAttackPowerAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetPhysicalAttackPowerAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetDefenseAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetDamageBonusPctAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);;
	}
	else if (Attribute == GetDamageReductionPctAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);;
	}
	else if (Attribute == GetStaggerDistanceAttribute())
	{
		if (NewValue > 1.f)
		{
			UCancerStaticsSubsystem::StaticStagger(GetCancerAbilitySystemComponent()->GetOwner());
		}
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetStaggerTimeAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetImpactAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetIronBodyAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	
}
