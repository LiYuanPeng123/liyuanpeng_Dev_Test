#include "CancerInventorySubsystem.h"
#include "CancerAssetManager.h"
#include "CancerInventorySetting.h"
#include "InventoryItemDefinition.h"

static TMap<FGameplayTag, TSubclassOf<UInventoryItemDefinition>> GetInventoryItemInfoMap(
	TArray<UDataTable*> InDataTables, TMap<int32, FGameplayTag>& InventoryItemIdToTag)
{
	TMap<FGameplayTag, TSubclassOf<UInventoryItemDefinition>> InfoMap;

	if (InDataTables.IsEmpty()) return InfoMap;

	for (auto Data : InDataTables)
	{
		TArray<FInventoryItemInfo*> InfoArray;
		Data->GetAllRows(FString(), InfoArray);
		for (const auto& Info : InfoArray)
		{
			check(Info->ItemDef);
			InventoryItemIdToTag.Add(Info->ItemID, Info->ItemTag);
			InfoMap.Add(Info->ItemTag, Info->ItemDef);
		}
	}
	return InfoMap;
}


static TMap<FGameplayTag, UCancerInteractionDefinition*> GetInventoryItemPickDefMap(
	TArray<UDataTable*> InDataTables, TMap<int32, FGameplayTag>& InventoryItemIdToTag)
{
	TMap<FGameplayTag, UCancerInteractionDefinition*> InfoMap;

	if (InDataTables.IsEmpty()) return InfoMap;

	for (auto Data : InDataTables)
	{
		TArray<FInventoryItemInfo*> InfoArray;
		Data->GetAllRows(FString(), InfoArray);
		for (const auto& Info : InfoArray)
		{
			check(Info->ItemDef);
			InventoryItemIdToTag.Add(Info->ItemID, Info->ItemTag);
			InfoMap.Add(Info->ItemTag, Info->PickupDef);
		}
	}
	return InfoMap;
}

void UCancerInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TArray<UDataTable*> ItemDataTable;
	for (auto Data : UCancerInventorySetting::Get()->ItemDataTables)
	{
		ItemDataTable.Add(UCancerAssetManager::GetAsset(Data));
	}
	InventoryItemMap = GetInventoryItemInfoMap(ItemDataTable, InventoryItemIdToTag);
	InventoryItemPickDefMap = GetInventoryItemPickDefMap(ItemDataTable, InventoryItemIdToTag);
}

void UCancerInventorySubsystem::Deinitialize()
{
	InventoryItemMap.Empty();
	InventoryItemIdToTag.Empty();
	InventoryItemPickDefMap.Empty();
	Super::Deinitialize();
}

TSubclassOf<UInventoryItemDefinition> UCancerInventorySubsystem::FindInventoryDef(FGameplayTag ItemTag)
{
	if (InventoryItemMap.Find(ItemTag))
	{
		return InventoryItemMap[ItemTag];
	}
	return nullptr;
}

UCancerInteractionDefinition* UCancerInventorySubsystem::FindInventoryPickupDef(FGameplayTag ItemTag)
{
	if (InventoryItemPickDefMap.Find(ItemTag))
	{
		return InventoryItemPickDefMap[ItemTag];
	}
	if (auto DefaultPickupDef = UCancerInventorySetting::Get()->PickupDefinition)
	{
		return DefaultPickupDef;
	}

	return nullptr;
}
