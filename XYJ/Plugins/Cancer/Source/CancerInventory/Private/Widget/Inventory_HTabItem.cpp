#include "Widget/Inventory_HTabItem.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Widget/Inventory_HTab.h"
DEFINE_LOG_CATEGORY_STATIC(LogTabItem, Log, All);

void UInventory_HTabItem::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventory_HTabItem::OnButtonClicked()
{
	CacheHTab->SetSelectedTab(TabIndex);
}

void UInventory_HTabItem::SetSelected(bool bSelected)
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

void UInventory_HTabItem::TabItemInit(const FName& InTabName, TSoftObjectPtr<UTexture2D> InImage,
                                      TSoftObjectPtr<UTexture2D> SelectedImage, int32 InTabIndex,
                                      UInventory_HTab* OwnerHTab)
{
	if (!OwnerHTab)
	{
		UE_LOG(LogTabItem, Error, TEXT("TabItemInit: OwnerHTab is null"));
		return;
	}
	TabButton->OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked);
	CacheHTab = OwnerHTab;
	TabIndex = InTabIndex;
	TabName = InTabName;
	if (!InImage.IsValid())
		Image_Item->SetVisibility(ESlateVisibility::Collapsed);
	Image_Item->SetBrushFromSoftTexture(InImage);
	Image_Selected->SetBrushFromSoftTexture(SelectedImage);
	TabNameText->SetText(FText::FromString(InTabName.ToString()));
}
