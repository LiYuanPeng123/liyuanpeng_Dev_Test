#include "CancerActorSubsystem.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Action_DataAssetTemplate.h"

void UCancerActorSubsystem::OnActorDistory(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	// 遍历所有Tag列表，移除该Actor
	for (auto It = ActorMap.CreateIterator(); It; ++It)
	{
		FCancerActorList& List = It.Value();
		List.Actors.Remove(Actor);

		// 如果列表为空，是否需要移除Key？视需求而定，这里暂时保留
	}
}

void UCancerActorSubsystem::RegisterActor(const UAction_DataAsset* ActorData, AActor* NewActor)
{
	if (!NewActor || !ActorData)
	{
		return;
	}

	FCancerActorList& List = ActorMap.FindOrAdd(ActorData->ActorTag);
	if (auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(NewActor))
	{
		ASC->AddLooseGameplayTag(ActorData->ActorTag);
		ASC->AddLooseGameplayTags(ActorData->ActorTagContainer);
	}
	if (!List.Actors.Contains(NewActor))
	{
		List.Actors.Add(NewActor);
		// 监听Actor销毁事件，以便自动移除
		if (!NewActor->OnEndPlay.IsAlreadyBound(this, &UCancerActorSubsystem::OnActorDistory))
		{
			NewActor->OnEndPlay.AddDynamic(this, &UCancerActorSubsystem::OnActorDistory);
		}
	}
	OnActorCreated.Broadcast(NewActor);
}


AActor* UCancerActorSubsystem::GetActor(const FGameplayTag& ActorTag)
{
	if (FCancerActorList* List = ActorMap.Find(ActorTag))
	{
		// 返回第一个有效的Actor
		for (AActor* Actor : List->Actors)
		{
			if (IsValid(Actor))
			{
				return Actor;
			}
		}
	}
	return nullptr;
}

TArray<AActor*> UCancerActorSubsystem::GetActors(const FGameplayTag& ActorTag)
{
	if (FCancerActorList* List = ActorMap.Find(ActorTag))
	{
		// 过滤掉无效的Actor（虽然OnEndPlay会移除，但多一层保障）
		TArray<AActor*> ValidActors;
		for (AActor* Actor : List->Actors)
		{
			if (IsValid(Actor))
			{
				ValidActors.Add(Actor);
			}
		}
		return ValidActors;
	}
	return TArray<AActor*>();
}

TArray<AActor*> UCancerActorSubsystem::FindCharacterByQuery(const TArray<FCancerQueryMatch>& TagQuery)
{
	TArray<AActor*> Result;
	TSet<AActor*> UniqueActors;

	// 遍历所有注册的Actor
	for (auto It = ActorMap.CreateConstIterator(); It; ++It)
	{
		const FCancerActorList& List = It.Value();
		for (AActor* AccessableActor : List.Actors)
		{
			if (IsValid(AccessableActor) && !UniqueActors.Contains(AccessableActor))
			{
				if (UCancerCoreFunctionLibrary::MatchAllQueryByActor(AccessableActor, TagQuery))
				{
					Result.Add(AccessableActor);
					UniqueActors.Add(AccessableActor);
				}
			}
		}
	}

	return Result;
}
