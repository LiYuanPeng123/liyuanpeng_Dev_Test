#include "Types/FActorPool.h"

#include "CancerActorPoolComponent.h"
#include "Interfaces/PoolableActorInterface.h"

FActorPoolList::FActorPoolList()
{
	PoolComponent = nullptr;
	TotalPoolSize = 0;
}

FActorPoolList::FActorPoolList(UCancerActorPoolComponent* PoolComponent)
	: PoolComponent(PoolComponent)
{
	TotalPoolSize = 0;
}

void FActorPoolList::SetPoolManager(UCancerActorPoolComponent* PoolManager)
{
	// 设置对象池管理组件（必须是有效组件）
	if (IsValid(PoolManager))
	{
		PoolComponent = PoolManager;
	}
}

void FActorPoolList::SetActorPoolSize(const TSubclassOf<AActor>& ActorClass, int32 PoolSize)
{
	if (!ActorPoolSize.Contains(ActorClass))
	{
		ActorPoolSize.Add(ActorClass, PoolSize);
		TotalPoolSize += PoolSize;
	}
}

void FActorPoolList::Initialize()
{
	Entries.Reserve(TotalPoolSize);
	check(PoolComponent);
	if (PoolComponent->GetOwner())
	{
		for (TPair<TSubclassOf<AActor>, int32> PoolEntry : ActorPoolSize)
		{
			const TSubclassOf<AActor>& ActorClass = PoolEntry.Key;
			for (int Idx = 0; Idx < PoolEntry.Value; ++Idx)
			{
				AddEntry(ActorClass, Idx);
			}
		}
	}
}

void FActorPoolList::AddEntry(const TSubclassOf<AActor>& ActorClass, int32 Order)
{
	if (!IsValid(ActorClass))
	{
		return;
	}

	const FGuid NewId = FGuid::NewGuid();

	FActorPoolEntry& Entry = Entries.AddDefaulted_GetRef();
	Entry.ActorClass = ActorClass;
	Entry.ActorPoolTag = IPoolableActorInterface::Execute_GetPoolableActorTag(ActorClass.GetDefaultObject());
	Entry.PooledActorId = NewId;
	Entry.Order = Order;
	Entry.AuthActor = PoolComponent->SpawnActor(ActorClass, NewId);
}

AActor* FActorPoolList::GetActor(const TSubclassOf<AActor>& ActorClass) const
{
	// 过滤出指定类且当前“未激活”的实例，便于借出复用
	TArray<FActorPoolEntry> ActorEntries = Entries.FilterByPredicate([ActorClass](const FActorPoolEntry& Entry)
	{
		const bool bHasValidAuthActor = Entry.AuthActor != nullptr && IPoolableActorInterface::Execute_IsInactive(
			Entry.AuthActor);
		return Entry.ActorClass == ActorClass
			&& bHasValidAuthActor;
	});

	Algo::Sort(ActorEntries, [](const FActorPoolEntry& A, const FActorPoolEntry& B)
	{
		return A.Order < B.Order;
	});

	if (ActorEntries.IsValidIndex(0))
	{
		return ActorEntries[0].AuthActor;
	}

	return nullptr;
}

AActor* FActorPoolList::GetActorByTag(const FGameplayTag& InTag) const
{
	TArray<FActorPoolEntry> ActorEntries = Entries.FilterByPredicate([InTag](const FActorPoolEntry& Entry)
	{
		const bool bHasValidAuthActor = Entry.AuthActor != nullptr && IPoolableActorInterface::Execute_IsInactive(
			Entry.AuthActor);
		return Entry.ActorPoolTag == IPoolableActorInterface::Execute_GetPoolableActorTag(
				Entry.AuthActor)
			&& bHasValidAuthActor;
	});

	Algo::Sort(ActorEntries, [](const FActorPoolEntry& A, const FActorPoolEntry& B)
	{
		return A.Order < B.Order;
	});

	if (ActorEntries.IsValidIndex(0))
	{
		return ActorEntries[0].AuthActor;
	}

	return nullptr;
}

void FActorPoolList::Empty()
{
	for (const FActorPoolEntry& Entry : Entries)
	{
		AActor* Actor = Entry.AuthActor;

		if (Actor && Actor->IsValidLowLevelFast())
		{
			IPoolableActorInterface::Execute_NotifyPoolDestroyed(Actor);
		}
	}
	// 清空本地条目，避免持有无效引用
	Entries.Empty();
}
