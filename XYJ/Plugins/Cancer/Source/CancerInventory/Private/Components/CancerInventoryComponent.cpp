#include "Components/CancerInventoryComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerInventoryFunctionLibrary.h"
#include "CancerNativeGamePlayTag.h"
#include "CancerQuickBarInstance.h"
#include "InventoryItemDefinition.h"
#include "InventoryItemInstance.h"
#include "Components/CancerQuickBarComponent.h"
#include "Fragment/InventoryFragment_SetCount.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "CancerInventory/Public/CancerInventorySubsystem.h"

//UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Inventory_Message_StackChanged, "Inventory.Message.StackChanged");


struct FCancerQueryMatch;
UE_DISABLE_OPTIMIZATION

DEFINE_LOG_CATEGORY(LogInventoryManagerComponent)

FString FInventoryEntry::GetDebugString() const
{
	TSubclassOf<UInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

void FInventoryList::BroadcastChangeMessage(FInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	GetInventoryManager()->OnInventoryItemChange.Broadcast(GetInventoryManager()->GetOwner(),
	                                                       Entry.Instance, OldCount, NewCount);

	FInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;
	//@TODO::广播被添加的消息
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(Tag_Inventory_Message_Stackchanged, Message);
}

// 添加或堆叠一条物品记录：
// - 若背包中不存在该标签条目，创建新实例并根据计数片段初始化堆叠数
// - 若已存在，根据计数片段（唯一/有上限/无限）决定实际可添加的数量
UInventoryItemInstance* FInventoryList::AddEntry(const FGameplayTag& ItemTag,
                                                 TSubclassOf<UInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);

	if (!Entries.Contains(ItemTag))
	{
		FInventoryEntry NewEntry;
		NewEntry.Instance = NewObject<UInventoryItemInstance>(OwnerComponent->GetOwner());
		NewEntry.Instance->SetItemDef(ItemDef);
		NewEntry.Instance->ItemTag = ItemTag;
		for (UInventoryItemFragment* Fragment : GetDefault<UInventoryItemDefinition>(ItemDef)->Fragments)
		{
			if (Fragment != nullptr)
			{
				Fragment->OnInstanceCreated(NewEntry.Instance);
			}
		}
		if (auto CountFragment = NewEntry.Instance->FindFragmentByClass<UInventoryFragment_SetCount>())
		{
			switch (CountFragment->ItemStack)
			{
			case EInventoryItemStack::None:
				StackCount = 1;
				break;
			case EInventoryItemStack::StackHasLimit:
				StackCount = FMath::Clamp(StackCount, 1, CountFragment->MaximumLimit);
				break;
			case EInventoryItemStack::StackNoLimit:
				StackCount = FMath::Max(StackCount, 1);
				break;
			default:
				break;
			}
		}

		NewEntry.StackCount = StackCount;
		Result = NewEntry.Instance;

		Entries.Add(ItemTag, NewEntry);

		BroadcastChangeMessage(NewEntry, 0, NewEntry.StackCount);
		return Result;
	}

	auto Entry = Entries.FindRef(ItemTag);
	Entry.LastObservedCount = Entry.StackCount;

	if (auto CountFragment = Entry.Instance->FindFragmentByClass<UInventoryFragment_SetCount>())
	{
		switch (CountFragment->ItemStack)
		{
		case EInventoryItemStack::None:
			StackCount = 0;
			break;
		case EInventoryItemStack::StackHasLimit:
			StackCount = FMath::Clamp(StackCount, 0, CountFragment->MaximumLimit - Entry.StackCount);
			break;
		case EInventoryItemStack::StackNoLimit:
			StackCount = FMath::Max(StackCount, 1);
			break;
		default:
			break;
		}
	}

	int32 OldCount = Entry.StackCount;
	Entry.StackCount = OldCount + StackCount;
	Entries[ItemTag] = Entry;
	Result = Entry.Instance;
	BroadcastChangeMessage(Entry, OldCount, Entry.StackCount);
	return Result;
	
}

UInventoryItemInstance* FInventoryList::AddEntry(const UInventoryItemInstance* ItemIns, int32 StackCount)
{
	FGameplayTag ItemTag = ItemIns->ItemTag;
	auto ItemDef = ItemIns->ItemDef;
	check(ItemDef != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);
	UInventoryItemInstance* Result = nullptr;
	if (!Entries.Contains(ItemTag))
	{
		FInventoryEntry NewEntry;
		NewEntry.Instance = NewObject<UInventoryItemInstance>(OwnerComponent->GetOwner());
		NewEntry.Instance->SetItemDef(ItemDef);
		NewEntry.Instance->ItemTag = ItemTag;
		for (UInventoryItemFragment* Fragment : GetDefault<UInventoryItemDefinition>(ItemDef)->Fragments)
		{
			if (Fragment != nullptr)
			{
				Fragment->OnInstanceCreated(NewEntry.Instance);
			}
		}
		if (auto CountFragment = NewEntry.Instance->FindFragmentByClass<UInventoryFragment_SetCount>())
		{
			switch (CountFragment->ItemStack)
			{
			case EInventoryItemStack::None:
				StackCount = 1;
				break;
			case EInventoryItemStack::StackHasLimit:
				StackCount = FMath::Clamp(StackCount, 1, CountFragment->MaximumLimit);
				break;
			case EInventoryItemStack::StackNoLimit:
				StackCount = FMath::Max(StackCount, 1);
				break;
			default:
				break;
			}
		}

		NewEntry.StackCount = StackCount;
		Result = NewEntry.Instance;

		Entries.Add(ItemTag, NewEntry);

		BroadcastChangeMessage(NewEntry, 0, NewEntry.StackCount);
		return Result;
	}

	auto Entry = Entries.FindRef(ItemTag);
	Entry.LastObservedCount = Entry.StackCount;

	if (auto CountFragment = Entry.Instance->FindFragmentByClass<UInventoryFragment_SetCount>())
	{
		switch (CountFragment->ItemStack)
		{
		case EInventoryItemStack::None:
			StackCount = 0;
			break;
		case EInventoryItemStack::StackHasLimit:
			StackCount = FMath::Clamp(StackCount, 0, CountFragment->MaximumLimit - Entry.StackCount);
			break;
		case EInventoryItemStack::StackNoLimit:
			StackCount = FMath::Max(StackCount, 1);
			break;
		default:
			break;
		}
	}

	int32 OldCount = Entry.StackCount;
	Entry.StackCount = OldCount + StackCount;
	Entries[ItemTag] = Entry;
	Result = Entry.Instance;

	BroadcastChangeMessage(Entry, OldCount, Entry.StackCount);
	return Result;
}


void FInventoryList::RemoveEntry(const FGameplayTag& ItemTag, int32 StackCount, bool bRemoveAll)
{
	if (!Entries.Contains(ItemTag))
	{
		return;
	}
	if (bRemoveAll)
	{
		auto Entry = Entries.FindRef(ItemTag);
		BroadcastChangeMessage(Entry, Entry.StackCount, 0);
		Entries.Remove(ItemTag);
		return;
	}

	auto Entry = Entries.FindRef(ItemTag);
	if (Entry.StackCount <= StackCount)
	{
		BroadcastChangeMessage(Entry, Entry.StackCount, 0);
		Entries.Remove(ItemTag);
		return;
	}
	Entry.LastObservedCount = Entry.StackCount;
	Entry.StackCount -= StackCount;
	Entries[ItemTag] = Entry;

	BroadcastChangeMessage(Entry, Entry.LastObservedCount, Entry.StackCount);
}


TArray<UInventoryItemInstance*> FInventoryList::GetAllItems() const
{
	TArray<UInventoryItemInstance*> Results;
	if (Entries.IsEmpty()) return Results;

	for (auto Entry : Entries)
	{
		if (Entry.Value.Instance != nullptr)
		{
			Results.Add(Entry.Value.Instance);
		}
	}
	return Results;
}


UInventoryItemInstance* FInventoryList::GetItemInstance(const FGameplayTag& ItemTag) const
{
	if (!Entries.Contains(ItemTag))
	{
		return nullptr;
	}
	return Entries.FindRef(ItemTag).Instance;
}


int32 FInventoryList::GetItemCount(const FGameplayTag& ItemTag) const
{
	if (!Entries.Contains(ItemTag))
	{
		return 0;
	}
	return Entries.FindRef(ItemTag).StackCount;
}


TArray<UInventoryItemInstance*> FInventoryList::FindItemsByTagMatch(const FGameplayTag& MatchTag) const
{
	TArray<UInventoryItemInstance*> Results;
	if (!MatchTag.IsValid() || Entries.IsEmpty())
	{
		return Results;
	}
	for (auto Entry : Entries)
	{
		if (Entry.Value.Instance != nullptr)
		{
			if (Entry.Key.MatchesTag(MatchTag))
			{
				Results.Add(Entry.Value.Instance);
			}
		}
	}
	return Results;
}

TArray<UInventoryItemInstance*> FInventoryList::FindItemsByTagQuery(const TArray<FCancerQueryMatch>& TagQuery) const
{
	TArray<UInventoryItemInstance*> Results;
	for (auto Entry : Entries)
	{
		if (Entry.Value.Instance != nullptr)
		{
			if (UCancerCoreFunctionLibrary::MatchAllQuery(TagQuery, FGameplayTagContainer(Entry.Key)))
			{
				Results.Add(Entry.Value.Instance);
			}
		}
	}
	return Results;
}


int32 FInventoryList::GetTotalItemCount(UInventoryItemInstance* Instance) const
{
	for (auto Entry : Entries)
	{
		if (Entry.Value.Instance == Instance)
		{
			return Entry.Value.StackCount;
		}
	}
	return 0;
}


int32 FInventoryList::GetTotalItemCount(const FGameplayTag& ItemTag) const
{
	if (!Entries.Contains(ItemTag))
	{
		return 0;
	}
	return Entries.FindRef(ItemTag).StackCount;
}


UCancerInventoryComponent::UCancerInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , InventoryList(this)
{
}


void UCancerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UCancerInventoryComponent::CanAddItem(const FGameplayTag& ItemTag, int32 StackCount)
{
	//@TODO:: 限制
	bool bResult = true;
	FString ResultStr = TEXT("道具添加检查");

	if (StackCount <= 0)
	{
		bResult = false;
		ResultStr = TEXT("用法错误-->添加道具");
	}

	const TMap<FGameplayTag, TSubclassOf<UInventoryItemDefinition>>* MapPtr = GetSubsystemInventoryItemMap();
	if (!MapPtr)
	{
		bResult = false;
		ResultStr = TEXT("Inventory Subsystem 不可用");
	}
	else
	{
		if (!MapPtr->Contains(ItemTag))
		{
			bResult = false;
			ResultStr = TEXT("DT表没有找到道具");
		}
		TSubclassOf<UInventoryItemDefinition> DefClass = MapPtr->FindRef(ItemTag);
		if (!DefClass)
		{
			bResult = false;
			ResultStr = TEXT("DT表-->") + ItemTag.ToString() + TEXT("没有配置Item定义");
		}
	}


	if (auto ItemInstance = InventoryList.GetItemInstance(ItemTag))
	{
		if (auto Fragment = ItemInstance->FindFragmentByClass<UInventoryFragment_SetCount>())
		{
			int32 OdlCount = InventoryList.GetItemCount(ItemTag);

			if (Fragment->ItemStack == EInventoryItemStack::None)
			{
				ResultStr = TEXT("唯一道具");
				bResult = false;
			}
			else if (Fragment->ItemStack == EInventoryItemStack::StackHasLimit)
			{
				int32 NewCount = StackCount + OdlCount;
				if (NewCount > Fragment->MaximumLimit)
				{
					ResultStr = TEXT("超过最大库存");
					bResult = false;
				}
			}
			else
			{
				bResult = true;
			}
		}
	}
	FString Str = bResult ? TEXT("成功") : TEXT("失败");
	UE_LOG(LogInventoryManagerComponent, Warning, TEXT("添加道具[%s] 添加数量[%d] 结果[%s] 原因[%s]"),
	       *ItemTag.ToString(), StackCount, *Str, *ResultStr);

	return bResult;
}


void UCancerInventoryComponent::RemoveItem(const FGameplayTag& ItemTag, int32 StackCount, bool bRemoveAll)
{
	InventoryList.RemoveEntry(ItemTag, StackCount, bRemoveAll);
}


bool UCancerInventoryComponent::AddItem(const FGameplayTag& ItemTag, int32 StackCount)
{
	bool bResult = false;
	if (CanAddItem(ItemTag, StackCount))
	{
		const TMap<FGameplayTag, TSubclassOf<UInventoryItemDefinition>>* MapPtr = GetSubsystemInventoryItemMap();
		TSubclassOf<UInventoryItemDefinition> DefClass = MapPtr ? MapPtr->FindRef(ItemTag) : nullptr;
		if (DefClass)
		{
			if (InventoryList.AddEntry(ItemTag, DefClass, StackCount))
			{
				bResult = true;
			}
		}
	}
	return bResult;
}

bool UCancerInventoryComponent::AddItemAndResult(const FGameplayTag& ItemTag, UInventoryItemInstance*& OutItemInstance,
                                                 int32 StackCount)
{
	bool bResult = false;
	if (CanAddItem(ItemTag, StackCount))
	{
		const TMap<FGameplayTag, TSubclassOf<UInventoryItemDefinition>>* MapPtr = GetSubsystemInventoryItemMap();
		TSubclassOf<UInventoryItemDefinition> DefClass = MapPtr ? MapPtr->FindRef(ItemTag) : nullptr;
		if (DefClass)
		{
			if (auto ItemIns = InventoryList.AddEntry(ItemTag, DefClass, StackCount))
			{
				OutItemInstance = ItemIns;
				bResult = true;
			}
		}
	}
	return bResult;
}

void UCancerInventoryComponent::AddItemIns(const UInventoryItemInstance* ItemIns, int32 StackCount)
{
	if (!ItemIns) return;
	if (CanAddItem(ItemIns->ItemTag, StackCount))
	{
		InventoryList.AddEntry(ItemIns, StackCount);
	}
}


UInventoryItemInstance* UCancerInventoryComponent::FindItem(const FGameplayTag& ItemTag) const
{
	return InventoryList.GetItemInstance(ItemTag);
}


TArray<UInventoryItemInstance*> UCancerInventoryComponent::FindItemsByTagMatch(const FGameplayTag& MatchTag) const
{
	return InventoryList.FindItemsByTagMatch(MatchTag);
}

TArray<UInventoryItemInstance*> UCancerInventoryComponent::FindItemsByTagQuery(const TArray<FCancerQueryMatch>& TagQuery
) const
{
	return InventoryList.FindItemsByTagQuery(TagQuery);
}

int32 UCancerInventoryComponent::EquipItemToQuickBar(const TArray<FCancerQueryMatch>& TagQuery,
                                                     FGameplayTag QuickBarTag)
{
	if (auto QuickBarComponent = UCancerInventoryFunctionLibrary::GetCancerQuickBarComponent(GetOwner()))
	{
		auto QuickBarIns = QuickBarComponent->GetQuickBarIns(QuickBarTag);
		if (!QuickBarIns)
		{
			return 0;
		}
		TArray<UInventoryItemInstance*> ItemInstances = FindItemsByTagQuery(TagQuery);
		if (ItemInstances.IsEmpty())
		{
			return 0;
		}
		int32 index = 0;
		for (auto ItemInstance : ItemInstances)
		{
			if (!QuickBarIns->AddItemToSlot(QuickBarIns->GetNextFreeItemSlot(), ItemInstance))
			{
				continue;
			}
			index++;
		}
		return index;
	}
	return 0;
}

TArray<UInventoryItemInstance*> UCancerInventoryComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}


int32 UCancerInventoryComponent::GetItemCount(const FGameplayTag& ItemTag) const
{
	return InventoryList.GetItemCount(ItemTag);
}


bool UCancerInventoryComponent::ConsumeItemsByDefinition(const FGameplayTag& ItemTag, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor)
	{
		return false;
	}

	int32 TotalConsumed = 0;
	return TotalConsumed == NumToConsume;
}

FGameplayTagContainer UCancerInventoryComponent::GetAllItemTags()
{
	FGameplayTagContainer Result;
	if (const TMap<FGameplayTag, TSubclassOf<UInventoryItemDefinition>>* MapPtr = GetSubsystemInventoryItemMap())
	{
		for (auto It : *MapPtr)
		{
			Result.AddTagFast(It.Key);
		}
	}
	return Result;
}

#pragma region 通过ID使用道具
bool UCancerInventoryComponent::CanAddItemByID(int32 ItemID, int32 StackCount)
{
	if (const TMap<int32, FGameplayTag>* IdMapPtr = GetSubsystemInventoryItemIdToTag())
	{
		if (IdMapPtr->Contains(ItemID))
		{
			return CanAddItem((*IdMapPtr)[ItemID], StackCount);
		}
	}
	return false;
}

void UCancerInventoryComponent::AddItemByID(int32 ItemID, int32 StackCount)
{
	if (const TMap<int32, FGameplayTag>* IdMapPtr = GetSubsystemInventoryItemIdToTag())
	{
		if (IdMapPtr->Contains(ItemID))
		{
			AddItem((*IdMapPtr)[ItemID], StackCount);
		}
	}
}

void UCancerInventoryComponent::RemoveItemByID(int32 ItemID, int32 StackCount, bool bRemoveAll)
{
	if (const TMap<int32, FGameplayTag>* IdMapPtr = GetSubsystemInventoryItemIdToTag())
	{
		if (IdMapPtr->Contains(ItemID))
		{
			RemoveItem((*IdMapPtr)[ItemID], StackCount, bRemoveAll);
		}
	}
}

const TMap<FGameplayTag, TSubclassOf<UInventoryItemDefinition>>*
UCancerInventoryComponent::GetSubsystemInventoryItemMap() const
{
	UCancerInventorySubsystem* Subsystem = GetWorld()
		                                       ? GetWorld()->GetGameInstance()->GetSubsystem<
			                                       UCancerInventorySubsystem>()
		                                       : nullptr;
	return Subsystem ? &Subsystem->InventoryItemMap : nullptr;
}

const TMap<int32, FGameplayTag>* UCancerInventoryComponent::GetSubsystemInventoryItemIdToTag() const
{
	UCancerInventorySubsystem* Subsystem = GetWorld()
		                                       ? GetWorld()->GetGameInstance()->GetSubsystem<
			                                       UCancerInventorySubsystem>()
		                                       : nullptr;
	return Subsystem ? &Subsystem->InventoryItemIdToTag : nullptr;
}


#pragma endregion
UE_ENABLE_OPTIMIZATION
