#include "AbilitySystem/Effects/CancerEffect_Dead.h"

#include "CancerNativeGamePlayTag.h"
#include "AbilitySystem/Effects/Requirements/CancerRequirement_Dead.h"
#include "GameplayEffectComponents/CustomCanApplyGameplayEffectComponent.h"
#include "GameplayEffectComponents/RemoveOtherGameplayEffectComponent.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UCancerEffect_Dead::UCancerEffect_Dead()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackLimitCount = 1;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::ClearEntireStack;


	FInheritedTagContainer EffectTags;
	EffectTags.Added.AddTagFast(Tag_Combat_Event_AbilityTrigger_Death);

	UTargetTagsGameplayEffectComponent* TargetTags = CreateDefaultSubobject<
		UTargetTagsGameplayEffectComponent>("TargetTags");
	TargetTags->SetAndApplyTargetTagChanges(EffectTags);
	GEComponents.Add(TargetTags);

	UCustomCanApplyGameplayEffectComponent* Requirement = CreateDefaultSubobject<
		UCustomCanApplyGameplayEffectComponent>("Requirement");
	Requirement->ApplicationRequirements.Add(UCancerRequirement_Dead::StaticClass());
	GEComponents.Add(Requirement);
}

void UCancerEffect_Dead::RemoveGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass) const
{
	FGameplayEffectQuery& Query = RemovedEffects->RemoveGameplayEffectQueries.AddDefaulted_GetRef();
	Query.EffectDefinition = EffectClass;
}
