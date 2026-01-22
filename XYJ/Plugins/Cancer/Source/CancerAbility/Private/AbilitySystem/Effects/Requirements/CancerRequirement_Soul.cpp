#include "AbilitySystem/Effects/Requirements/CancerRequirement_Soul.h"

#include "AbilitySystem/Attributes/CancerSoulSet.h"

UCancerRequirement_Soul::UCancerRequirement_Soul(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SoulAttribute = UCancerSoulSet::GetSoulAttribute();
}

bool UCancerRequirement_Soul::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
                                                                    const FGameplayEffectSpec& Spec,
                                                                    UAbilitySystemComponent* ASC) const
{
	if (!ASC->HasAttributeSetForAttribute(SoulAttribute))
	{
		return false;
	}

	const float Soul = ASC->GetNumericAttribute(SoulAttribute);
	return Soul <= 0.f;
}
