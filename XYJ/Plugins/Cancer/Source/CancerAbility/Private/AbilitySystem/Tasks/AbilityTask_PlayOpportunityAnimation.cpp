
#include "AbilitySystem/Tasks/AbilityTask_PlayOpportunityAnimation.h"
#include "ContextualAnimSceneActorComponent.h"
#include "ContextualAnimSceneAsset.h"
#include "ContextualAnimTypes.h"
#include "AbilitySystem/Interfaces/CombatOpportunityTargetInterface.h"
#include "Runtime/Launch/Resources/Version.h"

UAbilityTask_PlayOpportunityAnimation::UAbilityTask_PlayOpportunityAnimation()
{
	bEnableDebug = false;
}

UAbilityTask_PlayOpportunityAnimation* UAbilityTask_PlayOpportunityAnimation::CreateTask(
	UGameplayAbility* OwningAbility, UContextualAnimSceneAsset* SceneAsset, AActor* Attacker, AActor* Victim,
	const bool bEnableDebug)
{
	UAbilityTask_PlayOpportunityAnimation* NewTask = NewAbilityTask<UAbilityTask_PlayOpportunityAnimation>(OwningAbility);
	
	NewTask->bEnableDebug = bEnableDebug;
	NewTask->Attacker = Attacker;
	NewTask->Victim = Victim;
	NewTask->SceneAsset = SceneAsset;
	
	return NewTask;	
}

void UAbilityTask_PlayOpportunityAnimation::Activate()
{
	Super::Activate();

	if (!IsValid(Attacker) || !IsValid(Victim))
	{
		HandleSceneFailed();
		EndTask();
		return;
	}
	
	const UContextualAnimSceneAsset* ActualSceneAsset = SceneAsset;
	if (Victim->Implements<UCombatOpportunityTargetInterface>())
	{
		FGameplayTagContainer Tags;
		
#if ENGINE_MINOR_VERSION < 5
		Tags.AppendTags(Ability->AbilityTags);
#else
		Tags.AppendTags(Ability->GetAssetTags());
#endif
		
		const UContextualAnimSceneAsset* OverrideSceneAsset = ICombatOpportunityTargetInterface::Execute_GetContextualAnimSceneAsset(Victim, Tags);
		if (IsValid(OverrideSceneAsset))
		{
			ActualSceneAsset = OverrideSceneAsset; 
		}
	}

	if (!IsValid(ActualSceneAsset))
	{
		HandleSceneFailed();
		EndTask();
		return;
	}
	
	UContextualAnimSceneActorComponent* AnimComponent = Attacker->FindComponentByClass<UContextualAnimSceneActorComponent>();
	if (IsValid(AnimComponent))
	{
		AnimComponent->OnJoinedSceneDelegate.AddUniqueDynamic(this, &ThisClass::HandleAvatarJoinedScene);
		AnimComponent->OnLeftSceneDelegate.AddUniqueDynamic(this, &ThisClass::HandleAvatarLeftScene);
	}
	else
	{
		HandleSceneFailed();
		EndTask();
		return;
	}

	const FContextualAnimSceneBindingContext Primary(Attacker, Attacker->GetTransform());
	const FContextualAnimSceneBindingContext Secondary(Victim, Victim->GetTransform());

	static constexpr int32 SectionIdx = 0;
	FContextualAnimSceneBindings AnimSceneBindings;
	
	if (!FContextualAnimSceneBindings::TryCreateBindings(*SceneAsset, SectionIdx, Primary, Secondary, AnimSceneBindings))
	{
		HandleSceneFailed();
		EndTask();
		return;
	}

	if (!AdditionalSceneActors.IsEmpty() && AnimSceneBindings.IsValid())
	{
		for (TPair<FName, TObjectPtr<AActor>>& Entry : AdditionalSceneActors)
		{
			if (IsValid(Entry.Value))
			{
				AnimSceneBindings.BindActorToRole(*Entry.Value, Entry.Key);
			}
		}
	}

	if (AnimComponent->StartContextualAnimScene(AnimSceneBindings))
	{
		
	}
	else
	{
		HandleSceneFailed();
		EndTask();
	}
}

void UAbilityTask_PlayOpportunityAnimation::SetAdditionalSceneActors(TMap<FName, AActor*> ActorMap)
{
	if (ActorMap.IsEmpty())
	{
		return;
	}
	
	for (TPair<FName, AActor*>& Entry : ActorMap)
	{
		if (Entry.Key != NAME_None && IsValid(Entry.Value))
		{
			AdditionalSceneActors.Add(Entry.Key, Entry.Value);	
		}
	}
}

void UAbilityTask_PlayOpportunityAnimation::HandleAvatarJoinedScene(UContextualAnimSceneActorComponent* SceneActorComponent)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnJoinedScene.Broadcast(SceneActorComponent);
	}
}

void UAbilityTask_PlayOpportunityAnimation::HandleAvatarLeftScene(UContextualAnimSceneActorComponent* SceneActorComponent)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnLeftScene.Broadcast(SceneActorComponent);
	}
}

void UAbilityTask_PlayOpportunityAnimation::HandleSceneFailed()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnSceneFailed.Broadcast(nullptr);
	}
}
