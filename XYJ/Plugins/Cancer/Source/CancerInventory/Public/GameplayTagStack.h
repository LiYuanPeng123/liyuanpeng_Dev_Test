#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GameplayTagStack.generated.h"
struct FGameplayTagStackContainer;

USTRUCT(BlueprintType)
struct FGameplayTagStack
{
	GENERATED_BODY()

	FGameplayTagStack()
	{
	}

	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		  , StackCount(InStackCount)
	{
	}

	FString GetDebugString() const;

private:
	friend FGameplayTagStackContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

/** Container of gameplay tag stacks */
USTRUCT(BlueprintType)
struct FGameplayTagStackContainer
{
	GENERATED_BODY()

	FGameplayTagStackContainer()
	//	: Owner(nullptr)
	{
	}

public:
	void AddStack(FGameplayTag Tag, int32 StackCount);


	void RemoveStack(FGameplayTag Tag, int32 StackCount);


	int32 GetStackCount(FGameplayTag Tag) const
	{
		return TagToCountMap.FindRef(Tag);
	}

	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}

private:
	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;

	TMap<FGameplayTag, int32> TagToCountMap;
};
