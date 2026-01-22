#include "Widget/Inventory_ItemContainer.h"

#include "CancerInventoryFunctionLibrary.h"
#include "InventoryItemInstance.h"
#include "CancerUI/Public/Types/UIDefinition.h"
#include "Components/CancerInventoryComponent.h"
#include "Components/ScrollBox.h"
#include "Components/WrapBox.h"
#include "Widget/Inventory_Item.h"

UE_DISABLE_OPTIMIZATION

void UInventory_ItemContainer::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventory_ItemContainer::OnInitializedContainer_Implementation(AActor* Actor,
                                                                     TSubclassOf<UInventory_Item> ItemUIClass,
                                                                     int32 ItemNumber)
{
	OnInitializedInventoryContainer(Actor, ItemUIClass, ItemNumber);
}


void UInventory_ItemContainer::OnInitializedInventoryContainer(AActor* Actor, TSubclassOf<UInventory_Item> ItemUIClass,
                                                               int32 ItemNumber)
{
	if (!Actor || !ItemUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("初始化失败-- OnInitializedInventoryContainer "));
		return;
	}
	OwnerActor = Actor;
	//生成格子
	for (int32 i = 0; i < ItemNumber; i++)
	{
		auto Widget = CreateWidget<UInventory_Item>(GetWorld(), ItemUIClass);

		//初始化Item
		Widget->OnItemInitialized(this, i);
		InventoryContainer->AddChildToWrapBox(Widget);
		InventoryUIList.Add(Widget);
	}
}



void UInventory_ItemContainer::OnItemUpdate(const FGameplayTag& MatchTag)
{
	if (InventoryUIList.IsEmpty())
		return;
	auto InventoryComponent = GetInventoryComponent();
	if (!InventoryComponent)
	{
		return;
	}
	TArray<UInventoryItemInstance*> ItemInstances = InventoryComponent->FindItemsByTagMatch(MatchTag);
	

	for (int i = 0; i < InventoryUIList.Num(); i++)
	{
		if (ItemInstances.IsValidIndex(i))
		{
			InventoryUIList[i]->OnItemUpdated(ItemInstances[i]);
		}
		else
		{
			InventoryUIList[i]->OnItemUpdated(nullptr);
		}
	}
}

void UInventory_ItemContainer::SetSelectedItem(int32 NewSelectedIndex)
{
	if (InventoryUIList.IsEmpty())
		return;
	if (!InventoryUIList.IsValidIndex(NewSelectedIndex)) return;
	LastItemIndex = CurrentItemIndex;
	CurrentItemIndex = NewSelectedIndex;
	InventoryUIList[LastItemIndex]->SetSelected(false);
	InventoryUIList[CurrentItemIndex]->SetSelected(true);
	OnItemSelected.Broadcast(InventoryUIList[CurrentItemIndex]);
}

UCancerInventoryComponent* UInventory_ItemContainer::GetInventoryComponent() const
{
	if (OwnerActor)
	{
		return UCancerInventoryFunctionLibrary::GetCancerInventoryComponent(OwnerActor);
	}
	return nullptr;
}

void UInventory_ItemContainer::WASDSelected(const EDirectionType& Direction)
{
	if (InventoryUIList.IsEmpty())
		return;
	
	static int32 Col = 4;
	static int32 row = 5;
	int32 NewSelectedIndex = 0;

	//@TODO:背包格子为 行 5 列 4 共20
	if (Direction == EDirectionType::DT_None) return;
	if (Direction == EDirectionType::DT_Up)
	{
		if ((CurrentItemIndex - Col) < 0)
		{
			NewSelectedIndex = CurrentItemIndex - Col + Col * row;
		}
		else
		{
			NewSelectedIndex = CurrentItemIndex - Col;
		}
	}
	else if (Direction == EDirectionType::DT_Down)
	{
		if ((CurrentItemIndex + Col) > Col * row - 1)
		{
			NewSelectedIndex = (CurrentItemIndex + Col) - Col * row;
		}
		else
		{
			NewSelectedIndex = CurrentItemIndex + Col;
		}
	}
	else if (Direction == EDirectionType::DT_Left)
	{
		if ((CurrentItemIndex - 1) < 0)
		{
			NewSelectedIndex = 0;
		}
		else
		{
			NewSelectedIndex = CurrentItemIndex - 1;
		}
	}
	else
	{
		if ((CurrentItemIndex + 1) > Col * row - 1)
		{
			NewSelectedIndex = Col * row - 1;
		}
		else
		{
			NewSelectedIndex = CurrentItemIndex + 1;
		}
	}
	SetSelectedItem(NewSelectedIndex);
}

void UInventory_ItemContainer::SetScrollBoxOffset(int32 NewScrollBoxOffset)
{
	InventoryScrollBox->SetScrollOffset(NewScrollBoxOffset);
}

UE_ENABLE_OPTIMIZATION
