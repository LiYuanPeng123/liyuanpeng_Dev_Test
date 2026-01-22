#include "Fragment/InventoryFragment_SetImage.h"
#include "CancerGameSubsystem.h"
#include "Engine/Engine.h"

TSoftObjectPtr<UTexture2D> UInventoryFragment_SetImage::GetItemImage(UObject* WorldContextObject) const
{
	if (ImageSourceType == EImageSourceType::Direct)
	{
		return Image_Item;
	}
	else if (ImageSourceType == EImageSourceType::LookupKey && Image_ItemKey >= 0)
	{
		UWorld* World = nullptr;
		if (WorldContextObject)
		{
			World = WorldContextObject->GetWorld();
		}
		else
		{
			World = GetWorld();
		}
		
		if (World)
		{
			if (UCancerGameSubsystem* GameSubsystem = World->GetSubsystem<UCancerGameSubsystem>())
			{
				return GameSubsystem->GetImage(Image_ItemKey);
			}
		}
	}
	
	return nullptr;
}

TSoftObjectPtr<UTexture2D> UInventoryFragment_SetImage::GetSelectedBackImage(UObject* WorldContextObject) const
{
	if (ImageSourceType == EImageSourceType::Direct)
	{
		return Image_Item;
	}
	else if (ImageSourceType == EImageSourceType::LookupKey && Image_Selected_BackKey >= 0)
	{
		UWorld* World = nullptr;
		if (WorldContextObject)
		{
			World = WorldContextObject->GetWorld();
		}
		else
		{
			World = GetWorld();
		}
		
		if (World)
		{
			if (UCancerGameSubsystem* GameSubsystem = World->GetSubsystem<UCancerGameSubsystem>())
			{
				return GameSubsystem->GetImage(Image_Selected_BackKey);
			}
		}
	}
	
	return nullptr;
}
