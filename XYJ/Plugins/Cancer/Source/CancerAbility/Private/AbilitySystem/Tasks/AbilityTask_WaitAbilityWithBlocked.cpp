// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/AbilityTask_WaitAbilityWithBlocked.h"

#include "AbilitySystemComponent.h"


UAbilityTask_WaitAbilityWithBlocked::UAbilityTask_WaitAbilityWithBlocked(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IncludeTriggeredAbilities = false;
}

void UAbilityTask_WaitAbilityWithBlocked::Activate()
{
	if (AbilitySystemComponent.IsValid())
	{
		OnAbilityActivateDelegateHandle = AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this,
			&UAbilityTask_WaitAbilityWithBlocked::OnAbilityBlocked);
	}
}

void UAbilityTask_WaitAbilityWithBlocked::OnAbilityBlocked(const UGameplayAbility* BlockedAbility,
	const FGameplayTagContainer& FailureReason)
{
	if (!IncludeTriggeredAbilities && BlockedAbility->IsTriggered())
	{
		return;
	}

	const FGameplayTagContainer& AbilityTags = BlockedAbility->GetAssetTags();

	if (TagRequirements.IsEmpty())
	{
		if ((WithTag.IsValid() && !AbilityTags.HasTag(WithTag)) ||
			(WithoutTag.IsValid() && AbilityTags.HasTag(WithoutTag)))
		{
			// Failed tag check
			return;
		}
	}
	else
	{
		if (!TagRequirements.RequirementsMet(AbilityTags))
		{
			// Failed tag check
			return;
		}
	}

	if (Query.IsEmpty() == false)
	{
		if (Query.Matches(AbilityTags) == false)
		{
			// Failed query
			return;
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnBlocked.Broadcast(const_cast<UGameplayAbility*>(BlockedAbility));
	}

	if (TriggerOnce)
	{
		EndTask();
	}
}

UAbilityTask_WaitAbilityWithBlocked* UAbilityTask_WaitAbilityWithBlocked::WaitForAbilityBlocked(
	UGameplayAbility* OwningAbility, FGameplayTag InWithTag, FGameplayTag InWithoutTag, bool InIncludeTriggeredAbilities,
	bool InTriggerOnce)
{
	UAbilityTask_WaitAbilityWithBlocked* MyObj = NewAbilityTask<UAbilityTask_WaitAbilityWithBlocked>(OwningAbility);
	MyObj->WithTag = InWithTag;
	MyObj->WithoutTag = InWithoutTag;
	MyObj->IncludeTriggeredAbilities = InIncludeTriggeredAbilities;
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

UAbilityTask_WaitAbilityWithBlocked* UAbilityTask_WaitAbilityWithBlocked::WaitForAbilityBlockedWithTagRequirements(
	UGameplayAbility* OwningAbility, FGameplayTagRequirements TagRequirements, bool InIncludeTriggeredAbilities,
	bool InTriggerOnce)
{
	UAbilityTask_WaitAbilityWithBlocked* MyObj = NewAbilityTask<UAbilityTask_WaitAbilityWithBlocked>(OwningAbility);
	MyObj->TagRequirements = TagRequirements;
	MyObj->IncludeTriggeredAbilities = InIncludeTriggeredAbilities;
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

UAbilityTask_WaitAbilityWithBlocked* UAbilityTask_WaitAbilityWithBlocked::WaitForAbilityBlocked_Query(
	UGameplayAbility* OwningAbility, FGameplayTagQuery Query, bool InIncludeTriggeredAbilities, bool InTriggerOnce)
{
	UAbilityTask_WaitAbilityWithBlocked* MyObj = NewAbilityTask<UAbilityTask_WaitAbilityWithBlocked>(OwningAbility);
	MyObj->Query = Query;
	MyObj->IncludeTriggeredAbilities = InIncludeTriggeredAbilities;
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

void UAbilityTask_WaitAbilityWithBlocked::OnDestroy(bool AbilityEnded)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->AbilityActivatedCallbacks.Remove(OnAbilityActivateDelegateHandle);
	}

	Super::OnDestroy(AbilityEnded);
}
