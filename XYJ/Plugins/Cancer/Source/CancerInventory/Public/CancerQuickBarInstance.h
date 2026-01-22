#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/CancerInventoryComponent.h"
#include "UObject/Object.h"
#include "Components/CancerQuickBarComponent.h"
#include "CancerQuickBarInstance.generated.h"

class ACancerPickupActor;
class UCancerQuickBarComponent;
class UAbilitySystemComponent;
class UCancerEquipmentManagerComponent;
class UEquipmentInstance;
class UInventoryItemInstance;


//激活Slot变化 
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnQuickBarEquipmentEventDelegate, const FGameplayTag&, EquipmentTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickBarEquipmentChanged, const FGameplayTag&, EquipmentTag);

//Slot变化
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickBarSlotsAddorRemove, const TArray<UInventoryItemInstance*>&, Slots);

/**
 * 快捷切换栏实例
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class CANCERINVENTORY_API UCancerQuickBarInstance : public UObject
{
	GENERATED_BODY()

public:
	UCancerQuickBarInstance();

	virtual void RestoreQuickBarIns();

	UPROPERTY(BlueprintAssignable)
	FOnQuickBarEquipmentChanged OnQuickBarEquipmentChanged;

	UPROPERTY(BlueprintAssignable)
	FOnQuickBarSlotsAddorRemove OnQuickBarSlotsAddorRemove;

	virtual void BindToQuickBarEquipmentEvent(const FOnQuickBarEquipmentEventDelegate& Delegate);

	//背包道具发生变更
	UFUNCTION(BlueprintNativeEvent)
	void HandleInventoryItemChange(AActor* Owner, UInventoryItemInstance* ItemIns, int32 OldCount, int32 NewCount);


	void SetQuickBarOwner(AActor* InActor)
	{
		RestoreQuickBarIns();
		OwnerActor = InActor;
		if (Slots.Num() < NumSlots)
		{
			int32 Index = NumSlots - Slots.Num();
			Slots.AddDefaulted(Index);
			SlotsTag.AddDefaulted(Index);
		}
		if (UCancerInventoryComponent* Inventory = GetInventoryComponent())
		{
			Inventory->OnInventoryItemChange.AddDynamic(this, &UCancerQuickBarInstance::HandleInventoryItemChange);
		}
	};

	UFUNCTION(BlueprintCallable)
	void SetSlots(int32 NewSlots)
	{
		NumSlots = NewSlots;
		if (Slots.Num() < NumSlots)
		{
			int32 Index = NumSlots - Slots.Num();
			Slots.AddDefaulted(Index);
			SlotsTag.AddDefaulted(Index);
		}
	};

	// 向前轮换激活槽位
	UFUNCTION(BlueprintCallable, Category="")
	bool CycleActiveSlotForward();

	// 向后轮换激活槽位
	UFUNCTION(BlueprintCallable, Category="")
	bool CycleActiveSlotBackward();

	// 设置当前激活槽位索引
	UFUNCTION(BlueprintCallable, Category="")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, Category="")
	bool SetActiveSlotItem(const FGameplayTag& ItemTag);

	// 获取当前激活槽位索引（-1 表示未激活）
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure=true)
	UInventoryItemInstance* GetActiveSlotItem();

	UFUNCTION(BlueprintCallable, BlueprintPure=true)
	UInventoryItemInstance* GetSlotItemByIndex(int32 InSlotIndex);

	// 获取下一个空闲槽位下标（若无返回 INDEX_NONE）
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(BlueprintPure)
	bool IsEmpty(int32 SlotIndex);

	// 向指定槽位添加物品（仅在目标槽为空且 Item 非空时有效）
	UFUNCTION(BlueprintCallable)
	bool AddItemToSlot(int32 SlotIndex, UInventoryItemInstance* Item);

	// 自动选择槽位进行添加 无需指定Index
	void AddItem(UInventoryItemInstance* Item);

	// 卸下当前激活槽位中的已装备物品
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UnequipItemInSlot();

	// 装备当前激活槽位中的物品
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EquipItemInSlot();

	//使用道具
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ApplyItemInSlot();

	// 卸下当前激活槽位中的已装备物品 并且丢弃道具
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ACancerPickupActor* DiscardItemInSlot(const FTransform& Transform);

	UFUNCTION(BlueprintCallable)
	UInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	UInventoryItemInstance* RemoveItemFromItemTag(const FGameplayTag& ItemTag);

	FGameplayTag GetQuickBarTag() const { return QuickBarTag; };

	UFUNCTION(BlueprintCallable)
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable)
	UCancerQuickBarComponent* GetQuickBarComponent() const;

	UFUNCTION(BlueprintCallable)
	UCancerInventoryComponent* GetInventoryComponent() const;

	// 获取有东西的槽位数量
	UFUNCTION(BlueprintCallable)
	int32 GetValidSlots() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UInventoryItemInstance*> GetSlots()
	{
		return Slots;
	};
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FGameplayTag> GetSlotsTag() const
	{
		return SlotsTag;
	}

	UFUNCTION(BlueprintCallable)
	void SetSlotTag(int32 Index, const FGameplayTag& InTag)
	{
		if (SlotsTag.IsValidIndex(Index))
		{
			SlotsTag[Index] = InTag;
		}
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="快捷栏标签", meta=(Categories="快捷栏"))
	FGameplayTag QuickBarTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="插槽数量")
	int32 NumSlots = 3;

	UPROPERTY()
	TArray<TObjectPtr<UInventoryItemInstance>> Slots;

	UPROPERTY()
	TArray<FGameplayTag> SlotsTag;

	// 当前激活的槽位索引（-1 表示未激活）
	UPROPERTY()
	int32 ActiveSlotIndex = -1;

	TWeakObjectPtr<AActor> OwnerActor;
};
