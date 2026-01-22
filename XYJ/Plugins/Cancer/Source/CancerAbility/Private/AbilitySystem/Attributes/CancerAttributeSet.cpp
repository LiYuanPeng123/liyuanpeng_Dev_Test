
#include "AbilitySystem/Attributes/CancerAttributeSet.h"
#include "CancerAbilitySystemComponent.h"

UCancerAttributeSet::UCancerAttributeSet()
{
}

UWorld* UCancerAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UCancerAbilitySystemComponent* UCancerAttributeSet::GetCancerAbilitySystemComponent() const
{
	return Cast<UCancerAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
