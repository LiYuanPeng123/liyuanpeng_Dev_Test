#include "QuickBarIns/QuickBarIns_Weapon.h"
#include "CancerInventorySetting.h"
#include "CancerNativeGamePlayTag.h"
#include "InventoryItemInstance.h"
#include "Components/CancerEquipmentManagerComponent.h"
#include "Equipment/EquipmentDefinition.h"

#include "Fragment/InventoryFragment_EquippableItem.h"
#include "GameFramework/CancerPickupActor.h"

class UInventoryFragment_EquippableItem;

UQuickBarIns_Weapon::UQuickBarIns_Weapon()
{
	QuickBarTag = QuickBar::QuickBar_Weapon;
}

UCancerEquipmentManagerComponent* UQuickBarIns_Weapon::FindEquipmentManager() const
{
	return UCancerInventoryFunctionLibrary::GetCancerEquipmentManagerComponent(OwnerActor.Get());
}

EEquipSocket UQuickBarIns_Weapon::GetCurrentEquipItemSocket()
{
	if (auto EuqipIns = GetEquipItem())
	{
		return EuqipIns->GetCurrentEquipSocket();
	}
	return EEquipSocket::Default;
}


void UQuickBarIns_Weapon::EquipItemInSlot_Implementation()
{
	// 将当前激活槽位内的可装备物品交给装备管理组件进行穿戴
	check(Slots.IsValidIndex(ActiveSlotIndex));

	if (UInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		if (const UInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<
			UInventoryFragment_EquippableItem>())
		{
			TSubclassOf<UEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef != nullptr)
			{
				if (UCancerEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					EquippedItem = EquipmentManager->EquipItem(EquipDef);
					if (EquippedItem != nullptr)
					{
						//切换对应武技
						if (auto QuickComponent = GetQuickBarComponent())
						{
							if (auto QuickBarIns = QuickComponent->GetQuickBarIns(QuickBar::QuickBar_MilitaryClassics))
							{
								QuickBarIns->EquipItemInSlot();
							}
						}
						
						// 记录触发者（来源物品实例），便于后续逻辑（如效果归属）
						EquippedItem->SetInstigator(SlotItem);
						OnQuickBarEquipmentChanged.Broadcast(SlotItem->ItemTag);
					}
				}
			}
		}
	}
}

void UQuickBarIns_Weapon::UnequipItemInSlot_Implementation()
{
	// 若当前存在已装备的实例，则通过装备管理组件执行卸载并清空指针
	if (UCancerEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (EquippedItem != nullptr)
		{
			EquipmentManager->UnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

void UQuickBarIns_Weapon::AttachEquipToSlot(EEquipSocket InEquipSocket)
{
	check(EquippedItem);
	if (EquippedItem != nullptr)
		EquippedItem->AttachEquipToSlot(InEquipSocket);
}

ACancerPickupActor* UQuickBarIns_Weapon::DiscardItemInSlot_Implementation(const FTransform& Transform)
{
	UInventoryItemInstance* Result = nullptr;

	//TODO::默认0号插槽不可丢弃
	if (!Slots.IsValidIndex(ActiveSlotIndex) || ActiveSlotIndex == 0)
		return nullptr;

	UnequipItemInSlot();
	Result = Slots[ActiveSlotIndex];
	if (Result != nullptr)
	{
		Slots[ActiveSlotIndex] = nullptr;
	}
	SetActiveSlotIndex(0);
	
	if (Result)
	{
		if (auto InventoryComponent = GetInventoryComponent())
		{
			InventoryComponent->RemoveItem(Result->ItemTag);
		}
	}
	OnQuickBarSlotsAddorRemove.Broadcast(Slots);
	
	bool IsValid = false;
	return UCancerInventoryFunctionLibrary::K2_SpawnItem(OwnerActor.Get(), Result->ItemTag,
	                                                     Transform,
	                                                     IsValid, UCancerInventorySetting::Get()->PickupActorClass);
}
