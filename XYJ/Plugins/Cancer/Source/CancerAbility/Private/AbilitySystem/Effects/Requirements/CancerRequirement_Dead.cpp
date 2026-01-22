

#include "AbilitySystem/Effects/Requirements/CancerRequirement_Dead.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/CancerHealthSet.h"

UCancerRequirement_Dead::UCancerRequirement_Dead(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	HealthAttribute = UCancerHealthSet::GetHealthAttribute();
}

bool UCancerRequirement_Dead::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
	const FGameplayEffectSpec& Spec, UAbilitySystemComponent* ASC) const
{
	if (!ASC->HasAttributeSetForAttribute(HealthAttribute))
	{
		return false;
	}

	const float Health = ASC->GetNumericAttribute(HealthAttribute);
	return Health <= 0.f;
}
