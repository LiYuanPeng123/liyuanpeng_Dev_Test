
#include "Actions/AssetDefinition_ItemDefinition.h"
#include "InventoryItemDefinition.h"

UAssetDefinition_ItemDefinition::UAssetDefinition_ItemDefinition()
{
}

FText UAssetDefinition_ItemDefinition::GetAssetDisplayName() const
{
	return NSLOCTEXT("ItemDefinition", "ItemDef", "ItemDef");
}

FLinearColor UAssetDefinition_ItemDefinition::GetAssetColor() const
{
	return FLinearColor(0.2f, 0.8f, 0.4f);;
}

TSoftClassPtr<UObject> UAssetDefinition_ItemDefinition::GetAssetClass() const
{
	return UInventoryItemDefinition::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_ItemDefinition::GetAssetCategories() const
{
	static const auto Categories = {
		FAssetCategoryPath(NSLOCTEXT("Cancer", "Cancer", "Cancer"), NSLOCTEXT("Cancer", "Inventory", "Inventory"))
	};
	return Categories;
}
