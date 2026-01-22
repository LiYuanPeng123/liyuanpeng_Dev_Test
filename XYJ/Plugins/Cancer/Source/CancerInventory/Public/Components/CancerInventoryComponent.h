#pragma once
#include "CoreMinimal.h"
#include "CancerCoreFunctionLibrary.h"
#include "InventoryItemDefinition.h"
#include "Components/ActorComponent.h"
#include "Data/CancerAbilitySet.h"
#include "CancerInventoryComponent.generated.h"

struct FGameplayTag;
struct FInventoryItemInfo;
struct FInventoryList;
class UInventoryItemDefinition;
class UCancerInventoryComponent;
class UInventoryItemInstance;

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryManagerComponent, Log, All);

/**
 * 背包变化消息结构（示例：物品堆叠数变化）
 * - InventoryOwner：持有背包的组件（或其拥有的 Actor）。
 * - Instance：发生变化的物品实例。
 * - NewCount：变化后的数量。
 * - Delta：相对变化值（NewCount - OldCount）。
 */
USTRUCT(BlueprintType)
struct FInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};


//背包中单个物品条目
// 描述：维护某一“物品标签”对应的运行时实例与其堆叠信息。
USTRUCT(BlueprintType)
struct FInventoryEntry
{
	GENERATED_BODY()

	FInventoryEntry()
	{
	}

	FString GetDebugString() const;

private:
	friend FInventoryList;
	friend UCancerInventoryComponent;

	// 物品实例（运行时对象，承载该物品的所有片段/属性）
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInventoryItemInstance> Instance = nullptr;

	// 当前堆叠数量（对可堆叠物品有效）
	UPROPERTY(EditAnywhere)
	int32 StackCount = 0;

	// 上一次观测到的数量（用于变化对比与事件发布）
	UPROPERTY()
	int32 LastObservedCount = 0;
};

//背包列表
// 描述：以 GameplayTag 作为键，管理背包中的所有物品条目，并提供查询/添加/移除等方法。
USTRUCT(BlueprintType)
struct FInventoryList
{
	GENERATED_BODY()

	FInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	// 获取所有物品实例（过滤空指针）
	TArray<UInventoryItemInstance*> GetAllItems() const;

	// 精确标签查询对应的物品实例（不存在则返回 nullptr）
	UInventoryItemInstance* GetItemInstance(const FGameplayTag& ItemTag) const;

	// 精确标签查询堆叠数量（不存在返回 0）
	int32 GetItemCount(const FGameplayTag& ItemTag) const;

	// 标签匹配（MatchesTag）查询一组物品实例（常用于父标签/分类查询）
	TArray<UInventoryItemInstance*> FindItemsByTagMatch(const FGameplayTag& MatchTag) const;

	TArray<UInventoryItemInstance*> FindItemsByTagQuery(const TArray<FCancerQueryMatch>& TagQuery) const;

	// 通过实例查询其在背包中的总数量（通常等于所在条目的堆叠数）
	int32 GetTotalItemCount(UInventoryItemInstance* Instance) const;

	// 通过标签查询其总数量（与 GetItemCount 语义一致）
	int32 GetTotalItemCount(const FGameplayTag& ItemTag) const;

public:
	// 新增/堆叠条目（内部根据计数片段限制实际添加数量）
	UInventoryItemInstance* AddEntry(const FGameplayTag& ItemTag, TSubclassOf<UInventoryItemDefinition> ItemClass,
	                                 int32 StackCount);

	UInventoryItemInstance* AddEntry(const UInventoryItemInstance* ItemIns, int32 StackCount);

	// 移除条目：bRemoveAll=true 直接移除，否则按数量扣减
	void RemoveEntry(const FGameplayTag& ItemTag, int32 StackCount, bool bRemoveAll);

private:
	// 广播数量变化消息（供 UI/其它系统订阅）
	void BroadcastChangeMessage(FInventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:
	friend UCancerInventoryComponent;

private:
	// 背包条目表：Key 为物品标签，Value 为该标签的条目数据
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FInventoryEntry> Entries;

	// 背包拥有者组件（用于确定 Outer、广播消息、上下文访问等）
	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;

	UCancerInventoryComponent* GetInventoryManager() const
	{
		return static_cast<UCancerInventoryComponent*>(OwnerComponent);
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FInventoryItemChange, AActor*, Onwer, UInventoryItemInstance*, ItemIns
                                              , int32, OldCount, int32, NewCount);

UCLASS(ClassGroup=(Custom), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class CANCERINVENTORY_API UCancerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCancerInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

#pragma region
	UPROPERTY(BlueprintAssignable)
	FInventoryItemChange OnInventoryItemChange;
	// 是否可添加指定标签的物品（检查数量、数据表存在性、堆叠规则等）
	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool CanAddItem(const FGameplayTag& ItemTag, int32 StackCount = 1);

	// 从背包移除指定数量的物品；bRemoveAll=true 直接移除该标签整条条目
	UFUNCTION(BlueprintCallable, Category=Inventory)
	void RemoveItem(const FGameplayTag& ItemTag, int32 StackCount = 1, bool bRemoveAll = false);

	// 向背包添加物品（会先调用 CanAddItem 校验）
	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool AddItem(const FGameplayTag& ItemTag, int32 StackCount = 1);
	
	bool AddItemAndResult(const FGameplayTag& ItemTag, UInventoryItemInstance*& ItemInstance, int32 StackCount = 1);
	
	UFUNCTION(BlueprintCallable, Category=Inventory)
	void AddItemIns(const UInventoryItemInstance* ItemIns, int32 StackCount = 1);

	// 精确标签查找对应的物品实例
	UFUNCTION(BlueprintCallable, Category=Inventory)
	UInventoryItemInstance* FindItem(const FGameplayTag& ItemTag) const;

	// 标签匹配查找一组物品实例（返回数组）
	UFUNCTION(BlueprintPure, Category =Inventory, meta = (ReturnDisplayName = "Items"))
	TArray<UInventoryItemInstance*> FindItemsByTagMatch(const FGameplayTag& MatchTag) const;

	// 多次标签匹配 查找一组物品实例（返回数组）
	UFUNCTION(BlueprintPure, Category =Inventory, meta = (ReturnDisplayName = "Items"))
	TArray<UInventoryItemInstance*> FindItemsByTagQuery(const TArray<FCancerQueryMatch>& TagQuery) const;

	//标签匹配查找一组物品实例 并添加到快捷槽
	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 EquipItemToQuickBar(const TArray<FCancerQueryMatch>& TagQuery, FGameplayTag QuickBarTag);

	// 返回背包中所有物品实例
	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<UInventoryItemInstance*> GetAllItems() const;

	// 返回指定标签的当前数量
	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	int32 GetItemCount(const FGameplayTag& ItemTag) const;

	// 消耗指定标签的物品数量（占位接口，具体逻辑见 cpp）
	bool ConsumeItemsByDefinition(const FGameplayTag& ItemTag, int32 NumToConsume);

#pragma endregion
	UFUNCTION(BlueprintCallable, Category=Inventory)
	FGameplayTagContainer GetAllItemTags();

#pragma region 通过ID添加移除道具
	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool CanAddItemByID(int32 ItemID, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void AddItemByID(int32 ItemID, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void RemoveItemByID(int32 ItemID, int32 StackCount = 1, bool bRemoveAll = false);
#pragma endregion

private:
	const TMap<FGameplayTag, TSubclassOf<UInventoryItemDefinition>>* GetSubsystemInventoryItemMap() const;
	const TMap<int32, FGameplayTag>* GetSubsystemInventoryItemIdToTag() const;

	UPROPERTY()
	FInventoryList InventoryList;

	UPROPERTY()
	FInventoryList InventoryQuickList;
};
