
#include "Widget/Inventory_Item.h"

#include "InventoryItemInstance.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Widget/Inventory_ItemContainer.h"

void UInventory_Item::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ItemButton->OnClicked.AddDynamic(this,&ThisClass::OnItemButtonClicked);
	ItemButton->OnHovered.AddDynamic(this,&ThisClass::OnItemHovered);
}

void UInventory_Item::OnItemInitialized(UInventory_ItemContainer* InContainer, int32 InItemIndex)
{
	Container = InContainer;
	ItemIndex = InItemIndex;
	Image_Selected->SetVisibility(ESlateVisibility::Collapsed);
	Image_Item->SetVisibility(ESlateVisibility::Collapsed);
	ItemNumber->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventory_Item::OnItemUpdated_Implementation(UInventoryItemInstance* Item)
{
	
}

void UInventory_Item::SetSelected(bool bSelected)
{
	if (bSelected)
	{
		Image_Selected->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		Image_Selected->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventory_Item::ItemReset()
{
	ItemNumber->SetVisibility(ESlateVisibility::Collapsed);
	Image_Selected->SetVisibility(ESlateVisibility::Collapsed);
	Image_Item->SetVisibility(ESlateVisibility::Collapsed);
	Image_Back->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UInventory_Item::OnItemButtonClicked()
{
	if (CacheItemInstance)
	{
		UE_LOG(LogTemp,Warning,TEXT("ItemButtonClicked:Tag --> [%s]"),*CacheItemInstance->ItemTag.ToString());
	}
}

void UInventory_Item::OnItemHovered()
{
	Container->SetSelectedItem(ItemIndex);
}
