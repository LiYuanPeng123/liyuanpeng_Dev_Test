
#include "Actions/AssetDefinition_EquipmentDefinition.h"

#include "Equipment/EquipmentDefinition.h"

UAssetDefinition_EquipmentDefinition::UAssetDefinition_EquipmentDefinition()
{
}

FText UAssetDefinition_EquipmentDefinition::GetAssetDisplayName() const
{
	return NSLOCTEXT("EquipmentDefinition", "EquipmentDef", "EquipmentDef");
}

FLinearColor UAssetDefinition_EquipmentDefinition::GetAssetColor() const
{
	return FLinearColor(0.2f, 0.8f, 0.4f);;
}

TSoftClassPtr<UObject> UAssetDefinition_EquipmentDefinition::GetAssetClass() const
{
	return UEquipmentDefinition::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_EquipmentDefinition::GetAssetCategories() const
{
	static const auto Categories = {
		FAssetCategoryPath(NSLOCTEXT("Cancer", "Cancer", "Cancer"), NSLOCTEXT("Cancer", "Inventory", "Inventory"))
	};
	return Categories;
}
