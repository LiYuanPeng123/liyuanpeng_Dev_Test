#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryItemDefinition.generated.h"

class UCancerInteractionDefinition;
//片段定义
class UInventoryItemInstance;

UCLASS(MinimalAPI, DefaultToInstanced, EditInlineNew, Abstract)
class UInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const
	{
	}
};

//道具定义
UCLASS(Blueprintable, Const, Abstract)
class CANCERINVENTORY_API UInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UInventoryItemFragment>> Fragments;

public:
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;
};

//数据表格结构
USTRUCT(Blueprintable)
struct FInventoryItemInfo : public FTableRowBase
{
	GENERATED_BODY()

	//道具ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=ItemInfo)
	int32 ItemID = 0;

	//道具标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=ItemInfo, meta=(Categories="道具"))
	FGameplayTag ItemTag;

	//道具定义
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=ItemInfo)
	TSubclassOf<UInventoryItemDefinition> ItemDef;

	//道具拾取设置
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=ItemInfo)
	TObjectPtr<UCancerInteractionDefinition> PickupDef;
};

//物品等级
UENUM(BlueprintType)
enum class EInventoryItemLevel : uint8
{
	Common UMETA(DisplayName = "普通"),
	Uncommon UMETA(DisplayName = "优秀"),
	Rare UMETA(DisplayName = "稀有"),
	Epic UMETA(DisplayName = "史诗"),
	Legendary UMETA(DisplayName = "传说"),
};

//物品堆叠
UENUM(BlueprintType)
enum class EInventoryItemStack : uint8
{
	None UMETA(DisplayName="唯一"),
	StackNoLimit UMETA(DisplayName="无限制"),
	StackHasLimit UMETA(DisplayName="限制")
};
