#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FActorPool.generated.h"


class UCancerActorPoolComponent;

USTRUCT()
struct FActorPoolEntry
{
	GENERATED_BODY()
	UPROPERTY()
	TSubclassOf<AActor> ActorClass;

	UPROPERTY()
	FGameplayTag ActorPoolTag;

	UPROPERTY()
	FGuid PooledActorId = FGuid();

	UPROPERTY()
	int32 Order = INDEX_NONE;

	UPROPERTY()
	TObjectPtr<AActor> AuthActor = nullptr;
};


USTRUCT()
struct FActorPoolList
{
	GENERATED_BODY()

	FActorPoolList();
	explicit FActorPoolList(UCancerActorPoolComponent* PoolComponent);

	void SetPoolManager(UCancerActorPoolComponent* PoolManager);

	void SetActorPoolSize(const TSubclassOf<AActor>& ActorClass, int32 PoolSize);
	void Initialize();

	void AddEntry(const TSubclassOf<AActor>& ActorClass, int32 Order);

	AActor* GetActor(const TSubclassOf<AActor>& ActorClass) const;
	AActor* GetActorByTag(const FGameplayTag& InTag) const;
	void Empty();

private:
	int32 TotalPoolSize;

	UPROPERTY()
	TMap<TSubclassOf<AActor>, int32> ActorPoolSize;

	UPROPERTY()
	TObjectPtr<UCancerActorPoolComponent> PoolComponent;

	UPROPERTY()
	TArray<FActorPoolEntry> Entries;
};
