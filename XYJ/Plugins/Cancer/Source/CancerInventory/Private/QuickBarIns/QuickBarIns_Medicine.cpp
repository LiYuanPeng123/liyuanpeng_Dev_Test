#include "QuickBarIns/QuickBarIns_Medicine.h"

#include "AbilitySystemComponent.h"
#include "CancerNativeGamePlayTag.h"
#include "InventoryItemInstance.h"
#include "Fragment/InventoryItemFragment_ItemEffect.h"

class UInventoryFragment_AbilitySet;

UQuickBarIns_Medicine::UQuickBarIns_Medicine()
{
	QuickBarTag = QuickBar::QuickBar_Medicine;
}

void UQuickBarIns_Medicine::EquipItemInSlot_Implementation()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));

	if (UInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		OnQuickBarEquipmentChanged.Broadcast(SlotItem->ItemTag);
	}
}

bool UQuickBarIns_Medicine::ApplyItemInSlot_Implementation()
{
	if (!Slots.IsValidIndex(ActiveSlotIndex))
		return false;

	bool Result = true;
	if (UInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		auto ASC = GetAbilitySystemComponent();
		auto Inventory = GetInventoryComponent();
		if (const UInventoryItemFragment_ItemEffect* EffectFragment =
			SlotItem->FindFragmentByClass<UInventoryItemFragment_ItemEffect>())
		{
			for (auto Effect : EffectFragment->Effects)
			{
				if (!Effect.ItemEffect)
					continue;
				ASC->BP_ApplyGameplayEffectToSelf(Effect.ItemEffect, Effect.EffectLevel,
				                                  FGameplayEffectContextHandle());
			}
		}
		else
		{
			Result = false;
		}
		Inventory->RemoveItem(SlotItem->ItemTag, 1, false);
		if (Inventory->GetItemCount(SlotItem->ItemTag) <= 0)
		{
			RemoveItemFromItemTag(SlotItem->ItemTag);
			CycleActiveSlotBackward();
		}
	}
	return Result;
}
