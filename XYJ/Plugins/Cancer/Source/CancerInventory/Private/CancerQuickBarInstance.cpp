#include "CancerQuickBarInstance.h"

#include "AbilitySystemGlobals.h"
#include "CancerInventoryFunctionLibrary.h"
#include "InventoryItemInstance.h"


UE_DISABLE_OPTIMIZATION

UCancerQuickBarInstance::UCancerQuickBarInstance()
{
}

void UCancerQuickBarInstance::RestoreQuickBarIns()
{
	if (auto InventoryComponent = GetInventoryComponent())
	{
		InventoryComponent->OnInventoryItemChange.RemoveDynamic(this, &UCancerQuickBarInstance::HandleInventoryItemChange);
	}
	ActiveSlotIndex = -1;
	Slots.Reset();
	SlotsTag.Reset();
}

void UCancerQuickBarInstance::BindToQuickBarEquipmentEvent(const FOnQuickBarEquipmentEventDelegate& Delegate)
{
	OnQuickBarEquipmentChanged.Add(Delegate);
}

void UCancerQuickBarInstance::HandleInventoryItemChange_Implementation(AActor* Owner, UInventoryItemInstance* ItemIns,
                                                                       int32 OldCount, int32 NewCount)
{
	if (Slots.Contains(ItemIns))
	{
		if (NewCount <= 0)
		{
			RemoveItemFromItemTag(ItemIns->ItemTag);
			return;
		}
		OnQuickBarEquipmentChanged.Broadcast(ItemIns->ItemTag);
		return;
	}
	//插槽没有 标签存在说明之前这个插槽设置为这个Item 我们再次添加到这个插槽
	if (SlotsTag.Contains(ItemIns->ItemTag))
	{
		if (NewCount <= 0)
		{
			RemoveItemFromItemTag(ItemIns->ItemTag);
			return;
		}
		int32 SlotIndex = 0;
		for (auto Tag : SlotsTag)
		{
			if (ItemIns->ItemTag == Tag)
			{
				break;
			}
			SlotIndex++;
		}
		AddItemToSlot(SlotIndex, ItemIns);
	}
}

bool UCancerQuickBarInstance::CycleActiveSlotForward()
{
	// 向前轮换：从当前激活索引开始，循环查找下一个非空槽位
	if (Slots.Num() < 2)
	{
		return false;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return true;
		}
	}
	while (NewIndex != OldIndex);
	return false;
}

bool UCancerQuickBarInstance::CycleActiveSlotBackward()
{
	// 向后轮换：从当前激活索引开始，循环查找上一个非空槽位
	if (Slots.Num() < 2)
	{
		return false;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return true;
		}
	}
	while (NewIndex != OldIndex);
	return false;
}

void UCancerQuickBarInstance::SetActiveSlotIndex(int32 NewIndex)
{
	// 切换激活槽位：先卸下旧槽位装备，切换索引后装备新槽位物品
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		UnequipItemInSlot();
		ActiveSlotIndex = NewIndex;
		EquipItemInSlot();
	}
}

bool UCancerQuickBarInstance::SetActiveSlotItem(const FGameplayTag& ItemTag)
{
	int32 SlotIndex = 0;
	bool Result = false;
	for (const TObjectPtr<UInventoryItemInstance>& ItemPtr : Slots)
	{
		if (ItemPtr.Get())
		{
			if (ItemPtr.Get()->ItemTag == ItemTag)
			{
				Result = true;
				break;
			}
		}
		++SlotIndex;
	}
	if (Slots.IsValidIndex(SlotIndex))
	{
		SetActiveSlotIndex(SlotIndex);
	}
	return Result;
}

UInventoryItemInstance* UCancerQuickBarInstance::GetActiveSlotItem()
{
	if (Slots.IsValidIndex(ActiveSlotIndex))
		return Slots[ActiveSlotIndex].Get();
	return nullptr;
}

UInventoryItemInstance* UCancerQuickBarInstance::GetSlotItemByIndex(int32 InSlotIndex)
{
	if (Slots.IsValidIndex(InSlotIndex))
		return Slots[InSlotIndex].Get();
	return nullptr;
}

int32 UCancerQuickBarInstance::GetNextFreeItemSlot() const
{
	// 查找第一个为空的槽位索引，若没有空位返回 INDEX_NONE
	int32 SlotIndex = 0;
	for (const TObjectPtr<UInventoryItemInstance>& ItemPtr : Slots)
	{
		if (ItemPtr == nullptr)
		{
			return SlotIndex;
		}
		++SlotIndex;
	}

	return INDEX_NONE;
}

bool UCancerQuickBarInstance::IsEmpty(int32 SlotIndex)
{
	if (Slots.IsValidIndex(SlotIndex))
	{
		return Slots[SlotIndex] == nullptr;
	}
	return false;
}

bool UCancerQuickBarInstance::AddItemToSlot(int32 SlotIndex, UInventoryItemInstance* Item)
{
	bool Return = false;
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr) && !Slots.Contains(Item))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
			//记录每次添加到插槽的道具 避免道具移除后 再次添加找不到插槽
			SlotsTag[SlotIndex] = Item->ItemTag;
			Return = true;
		}
	}
	OnQuickBarSlotsAddorRemove.Broadcast(Slots);
	return Return;
}

void UCancerQuickBarInstance::AddItem(UInventoryItemInstance* Item)
{
	auto Index = GetNextFreeItemSlot();
	if (Index != INDEX_NONE)
	{
		AddItemToSlot(Index, Item);
	}
	UE_LOG(LogTemp, Error, TEXT(" Too Many Item add to slot ! UCancerQuickBarInstance::AddItem"));
}

ACancerPickupActor* UCancerQuickBarInstance::DiscardItemInSlot_Implementation(const FTransform& Transform)
{
	UInventoryItemInstance* Result = nullptr;

	if (Slots.IsValidIndex(ActiveSlotIndex))
	{
		UnequipItemInSlot();
		Result = Slots[ActiveSlotIndex];

		if (Result != nullptr)
		{
			Slots[ActiveSlotIndex] = nullptr;
		}
		CycleActiveSlotBackward();
	}
	if (Result)
	{
		if (auto InventoryComponent = GetInventoryComponent())
		{
			InventoryComponent->RemoveItem(Result->ItemTag);
		}
	}
	OnQuickBarSlotsAddorRemove.Broadcast(Slots);
	return nullptr;
}

bool UCancerQuickBarInstance::ApplyItemInSlot_Implementation()
{
	return true;
}


UInventoryItemInstance* UCancerQuickBarInstance::RemoveItemFromSlot(int32 SlotIndex)
{
	// 从槽位移除物品并返回被移除的实例；若移除的是当前激活槽，则先执行卸载并重置激活索引
	UInventoryItemInstance* Result = nullptr;

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		ActiveSlotIndex = -1;
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
		}
	}
	OnQuickBarSlotsAddorRemove.Broadcast(Slots);
	return Result;
}

UInventoryItemInstance* UCancerQuickBarInstance::RemoveItemFromItemTag(const FGameplayTag& ItemTag)
{
	// 从槽位移除物品并返回被移除的实例；若移除的是当前激活槽，则先执行卸载并重置激活索引
	UInventoryItemInstance* Result = nullptr;

	int32 SlotIndex = 0;
	for (auto Item : Slots)
	{
		if (!Item)
		{
			SlotIndex++;
			continue;
		}
		if (Item.Get()->ItemTag == ItemTag)
		{
			break;
		}
		SlotIndex++;
	}
	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		ActiveSlotIndex = -1;
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
		}
	}
	OnQuickBarSlotsAddorRemove.Broadcast(Slots);
	return Result;
}

UAbilitySystemComponent* UCancerQuickBarInstance::GetAbilitySystemComponent() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor.Get());
}

UCancerQuickBarComponent* UCancerQuickBarInstance::GetQuickBarComponent() const
{
	return UCancerInventoryFunctionLibrary::GetCancerQuickBarComponent(OwnerActor.Get());
}

UCancerInventoryComponent* UCancerQuickBarInstance::GetInventoryComponent() const
{
	return UCancerInventoryFunctionLibrary::GetCancerInventoryComponent(OwnerActor.Get());
}

int32 UCancerQuickBarInstance::GetValidSlots() const
{
	int32 Num = 0;
	for (const auto& Slot : Slots)
	{
		if (Slot.Get() != nullptr)
		{
			++Num;
		}
	}
	return Num;
}

void UCancerQuickBarInstance::EquipItemInSlot_Implementation()
{
}

void UCancerQuickBarInstance::UnequipItemInSlot_Implementation()
{
}

UE_ENABLE_OPTIMIZATION
