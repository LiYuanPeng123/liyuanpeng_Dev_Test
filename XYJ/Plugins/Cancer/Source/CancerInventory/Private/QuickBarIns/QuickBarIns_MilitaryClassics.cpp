#include "QuickBarIns/QuickBarIns_MilitaryClassics.h"

#include "CancerNativeGamePlayTag.h"
#include "InventoryItemInstance.h"
#include "Components/CancerQuickBarComponent.h"
#include "Fragment/InventoryFragment_LinkSkillsItem.h"

UQuickBarIns_MilitaryClassics::UQuickBarIns_MilitaryClassics()
{
	QuickBarTag = QuickBar::QuickBar_MilitaryClassics;
}

void UQuickBarIns_MilitaryClassics::EquipItemInSlot_Implementation()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	
	if (auto QuickComponent = GetQuickBarComponent())
	{
		if (auto QuickBarIns = QuickComponent->GetQuickBarIns(QuickBar::QuickBar_Skill))
		{
			//卸下当前装备的武技
			QuickBarIns->UnequipItemInSlot();
			QuickBarIns->RestoreQuickBarIns();

			if (UInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
			{
				if (const UInventoryFragment_LinkSkillsItem* SkillsItem = SlotItem->FindFragmentByClass<
					UInventoryFragment_LinkSkillsItem>())
				{
					auto LinkItemTags = SkillsItem->LinkItemTags;
					auto InventoryComponent = GetInventoryComponent();
					check(InventoryComponent);

					if (auto ItemIns = QuickComponent->GetQuickBarIns(QuickBar::QuickBar_Weapon)->GetActiveSlotItem())
					{
						TArray<FGameplayTag> SkillTags;
						for (auto Pair : LinkItemTags)
						{
							if (ItemIns->ItemTag.MatchesTag(Pair.Key))
							{
								SkillTags = Pair.Value.GetGameplayTagArray();
							}
						}
						//重新初始化
						QuickBarIns->SetSlots(SkillTags.Num());
						int32 Index = 0;
						for (auto LinkItemTag : SkillTags)
						{
							auto Item = InventoryComponent->FindItem(LinkItemTag);
							QuickBarIns->AddItemToSlot(Index, Item);
							Index++;
						}
						QuickBarIns->SetActiveSlotIndex(0);
					}
					
				}
				OnQuickBarEquipmentChanged.Broadcast(SlotItem->ItemTag);
			}
		}
	}
}

void UQuickBarIns_MilitaryClassics::UnequipItemInSlot_Implementation()
{
	if (auto QuickComponent = GetQuickBarComponent())
	{
		if (auto QuickBarIns = QuickComponent->GetQuickBarIns(QuickBar::QuickBar_Skill))
		{
			//卸下当前装备的武技
			QuickBarIns->UnequipItemInSlot();
			QuickBarIns->RestoreQuickBarIns();
		}
	}
}
