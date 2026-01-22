#pragma once

#include "CoreMinimal.h"
#include "GameplayTagStack.h"
#include "InventoryItemInstance.generated.h"

struct FGameplayTag;
class UInventoryItemFragment;
class UInventoryItemDefinition;

/**
 * 道具实例
 */
UCLASS(BlueprintType, Category=Inventory)
class CANCERINVENTORY_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category= Inventory)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

	TSubclassOf<UInventoryItemDefinition> GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false,
		meta=(DeterminesOutputType=FragmentClass, ExpandBoolAsExecs="bIsValid"))
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass,
	                                                  bool& bIsValid) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		bool bIsValid = false;
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass(), bIsValid);
	}

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag ItemTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Inventory)
	int32 ItemID = 0;

private:
	void SetItemDef(TSubclassOf<UInventoryItemDefinition> InDef);

	friend struct FInventoryList;

private:
	UPROPERTY(EditAnywhere)
	FGameplayTagStackContainer StatTags;

	// The item definition
	UPROPERTY(EditAnywhere)
	TSubclassOf<UInventoryItemDefinition> ItemDef;
};
