
#include "QuickBarIns/QuickBarIns_Magic.h"

#include "CancerNativeGamePlayTag.h"
#include "InventoryItemInstance.h"
#include "Data/CancerAbilitySet.h"
#include "Fragment/InventoryFragment_AbilitySet.h"

UQuickBarIns_Magic::UQuickBarIns_Magic()
{
	QuickBarTag = QuickBar::QuickBar_Magic;
}

void UQuickBarIns_Magic::RestoreQuickBarIns()
{
	Super::RestoreQuickBarIns();
	GrantedHandle.ClearHandle();
}

void UQuickBarIns_Magic::EquipItemInSlot_Implementation()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	if (UInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		if (const UInventoryFragment_AbilitySet* AbilityInfo = SlotItem->FindFragmentByClass<
			UInventoryFragment_AbilitySet>())
		{
			if (auto ASC = GetAbilitySystemComponent())
			{
				AbilityInfo->AbilitySetToGrant->GiveToAbilitySystem(ASC, &GrantedHandle, OwnerActor.Get());
			}
		}

		OnQuickBarEquipmentChanged.Broadcast(SlotItem->ItemTag);
	}
}

void UQuickBarIns_Magic::UnequipItemInSlot_Implementation()
{
	//移除装备所赋予的技能
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		GrantedHandle.TakeFromAbilitySystem(ASC);
	}
}

