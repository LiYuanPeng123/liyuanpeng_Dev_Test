// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/TargetingFilterTask_HasTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

bool UTargetingFilterTask_HasTag::ShouldFilterTarget(const FTargetingRequestHandle& TargetingHandle,
                                                     const FTargetingDefaultResultData& TargetData) const
{
	if (AActor* TargetActor = TargetData.HitResult.GetActor())
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (AbilitySystemComponent->HasAllMatchingGameplayTags(TagContainer))
			{
				return false;
			}
		}
	}

	return true;
}
