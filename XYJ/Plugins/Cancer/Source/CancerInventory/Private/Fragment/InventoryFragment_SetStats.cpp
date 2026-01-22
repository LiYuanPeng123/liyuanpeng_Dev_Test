


#include "Fragment/InventoryFragment_SetStats.h"

#include "InventoryItemInstance.h"


void UInventoryFragment_SetStats::OnInstanceCreated(UInventoryItemInstance* Instance) const
{
	for (const auto& KVP : InitialItemStats)
	{
		FGameplayTag Tag = KVP.Key;
		int32 StackCount = KVP.Value;
		Instance->AddStatTagStack(Tag, StackCount);
	}
}

int32 UInventoryFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}
