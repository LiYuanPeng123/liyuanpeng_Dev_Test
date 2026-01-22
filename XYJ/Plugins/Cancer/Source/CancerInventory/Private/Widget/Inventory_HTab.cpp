#include "Widget/Inventory_HTab.h"

#include "Widget/Inventory_HTabItem.h"


void UInventory_HTab::SetSelectedTab(int32 InTabIndex)
{
	if (!InventoryItems.IsValidIndex(InTabIndex))return;

	LastSelectedTab = CurSelectedTab;
	CurSelectedTab = InTabIndex;

	if (InventoryItems[LastSelectedTab])
	{
		InventoryItems[LastSelectedTab]->SetSelected(false);
	}
	InventoryItems[CurSelectedTab]->SetSelected(true);

	TabChangedDelegate.Broadcast(InventoryItems[CurSelectedTab]);
}


void UInventory_HTab::OnInitializedHTab_Implementation()
{
}

const FName& UInventory_HTab::GetActiveTabName()
{
	return InventoryItems[CurSelectedTab]->TabName;
}

UItemTabInfoObject* UInventory_HTab::GetTabInfoByTableName(const FName& InTabName)
{
	if (TableItemInfoMap.Find(InTabName))
		return TableItemInfoMap[InTabName];
	return nullptr;
}

void UInventory_HTab::BackToPreviousOption_Implementation()
{
	int32 PreIndex = (CurSelectedTab - 1) < 0 ? InventoryItems.Num() - 1 : CurSelectedTab - 1;
	SetSelectedTab(PreIndex);
}

void UInventory_HTab::AdvanceToNextOption_Implementation()
{
	int32 PreIndex = (CurSelectedTab + 1) > InventoryItems.Num() - 1 ? 0 : (CurSelectedTab + 1);
	SetSelectedTab(PreIndex);
}
