#include "QuickBarIns/QuickBarIns_Skills.h"

#include "CancerNativeGamePlayTag.h"
#include "InventoryItemInstance.h"
#include "Data/CancerAbilitySet.h"
#include "Fragment/InventoryFragment_AbilitySet.h"

class UInventoryFragment_AbilitySet;

UQuickBarIns_Skills::UQuickBarIns_Skills()
{
	QuickBarTag = QuickBar::QuickBar_Skill;
}

void UQuickBarIns_Skills::RestoreQuickBarIns()
{
	Super::RestoreQuickBarIns();
	GrantedHandle.ClearHandle();
}

void UQuickBarIns_Skills::EquipItemInSlot_Implementation()
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

void UQuickBarIns_Skills::UnequipItemInSlot_Implementation()
{
	//移除装备所赋予的技能
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		GrantedHandle.TakeFromAbilitySystem(ASC);
	}
}
