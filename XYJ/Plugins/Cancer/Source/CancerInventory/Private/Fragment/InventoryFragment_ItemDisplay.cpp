

#include "Fragment/InventoryFragment_ItemDisplay.h"

#include "CancerGameSubsystem.h"

FName UInventoryFragment_ItemDisplay::GetItemName(const UObject* WorldContextObject)
{
	if (DisplaySourceType == EStrDisplaySourceType::LookupKey)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (UCancerGameSubsystem* GameSubsystem = World->GetSubsystem<UCancerGameSubsystem>())
			{
				return *GameSubsystem->GetTextDescription(ItemNameIndex);
			}
		}
	}
	return ItemName;
}

FName UInventoryFragment_ItemDisplay::GetDisplayName(const UObject* WorldContextObject)
{
	if (DisplaySourceType == EStrDisplaySourceType::LookupKey)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (UCancerGameSubsystem* GameSubsystem = World->GetSubsystem<UCancerGameSubsystem>())
			{
				return *GameSubsystem->GetTextDescription(ItemDisplayNameIndex);
			}
		}
		
	}
	return ItemDisplayName;
}

FName UInventoryFragment_ItemDisplay::GetItemName1(const UObject* WorldContextObject)
{
	if (DisplaySourceType == EStrDisplaySourceType::LookupKey)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (UCancerGameSubsystem* GameSubsystem = World->GetSubsystem<UCancerGameSubsystem>())
			{
				return *GameSubsystem->GetTextDescription(ItemNameIndex1);
			}
		}
	}
	return ItemName1;
}

FName UInventoryFragment_ItemDisplay::GetItemName2(const UObject* WorldContextObject)
{
	if (DisplaySourceType == EStrDisplaySourceType::LookupKey)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (UCancerGameSubsystem* GameSubsystem = World->GetSubsystem<UCancerGameSubsystem>())
			{
				return *GameSubsystem->GetTextDescription(ItemNameIndex2);
			}
		}
	}
	return ItemName2;
}

FName UInventoryFragment_ItemDisplay::GetItemName3(const UObject* WorldContextObject)
{
	if (DisplaySourceType == EStrDisplaySourceType::LookupKey)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (UCancerGameSubsystem* GameSubsystem = World->GetSubsystem<UCancerGameSubsystem>())
			{
				return *GameSubsystem->GetTextDescription(ItemNameIndex3);
			}
		}
	}
	return ItemName3;
}
