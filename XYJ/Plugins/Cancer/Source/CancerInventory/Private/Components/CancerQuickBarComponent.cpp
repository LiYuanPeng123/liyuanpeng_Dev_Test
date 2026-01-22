#include "Components/CancerQuickBarComponent.h"

#include "CancerNativeGamePlayTag.h"
#include "CancerQuickBarInstance.h"


UCancerQuickBarComponent::UCancerQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCancerQuickBarComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RestQuickBarIns();
	Super::EndPlay(EndPlayReason);
}

void UCancerQuickBarComponent::RestQuickBarIns()
{
	for (auto QuickBarIns : QuickBarInstances)
	{
		QuickBarIns->RestoreQuickBarIns();
	}
}


void UCancerQuickBarComponent::AddQuickBarInstance(UCancerQuickBarInstance* QuickBarIns)
{
	if (QuickBarIns == nullptr) return;
	QuickBarIns->SetQuickBarOwner(GetOwner());
	QuickBarInstances.Add(QuickBarIns);
}

UCancerQuickBarInstance* UCancerQuickBarComponent::GetQuickBarIns(const FGameplayTag& QuickBarTag)
{
	for (auto QuickBarIns : QuickBarInstances)
	{
		if (QuickBarIns->GetQuickBarTag() == QuickBarTag)
		{
			return QuickBarIns;
		}
	}
	return nullptr;
}

void UCancerQuickBarComponent::AddItemToSlot(const FGameplayTag& QuickBarTag, int32 SlotIndex,
                                             UInventoryItemInstance* Item)
{
	if (auto QuickBarIns = GetQuickBarIns(QuickBarTag))
	{
		QuickBarIns->AddItemToSlot(SlotIndex, Item);
	}
}

void UCancerQuickBarComponent::AddItem(const FGameplayTag& QuickBarTag, UInventoryItemInstance* Item)
{
	if (auto QuickBarIns = GetQuickBarIns(QuickBarTag))
	{
		QuickBarIns->AddItem(Item);
	}
}

UInventoryItemInstance* UCancerQuickBarComponent::RemoveItemFromSlot(const FGameplayTag& QuickBarTag, int32 SlotIndex)
{
	if (auto QuickBarIns = GetQuickBarIns(QuickBarTag))
	{
		return QuickBarIns->RemoveItemFromSlot(SlotIndex);
	}
	return nullptr;
}

UInventoryItemInstance* UCancerQuickBarComponent::RemoveItemFromItemTag(const FGameplayTag& QuickBarTag,
                                                                        const FGameplayTag& ItemTag)
{
	if (auto QuickBarIns = GetQuickBarIns(QuickBarTag))
	{
		return QuickBarIns->RemoveItemFromItemTag(ItemTag);
	}
	return nullptr;
}

void UCancerQuickBarComponent::ClearWeaponInstance()
{
	if (UCancerQuickBarInstance* QuickBarIns = GetQuickBarIns(QuickBar::QuickBar_Weapon))
	{
		QuickBarIns->RestoreQuickBarIns();
	}
}

bool UCancerQuickBarComponent::CycleActiveSlotForward(const FGameplayTag& QuickBarTag)
{
	if (auto QuickBarIns = GetQuickBarIns(QuickBarTag))
	{
		return QuickBarIns->CycleActiveSlotForward();
	}
	return false;
}

bool UCancerQuickBarComponent::CycleActiveSlotBackward(const FGameplayTag& QuickBarTag)
{
	if (auto QuickBarIns = GetQuickBarIns(QuickBarTag))
	{
		return QuickBarIns->CycleActiveSlotBackward();
	}
	return false;
}

void UCancerQuickBarComponent::SetActiveSlotIndex(const FGameplayTag& QuickBarTag, int32 NewIndex)
{
	if (auto QuickBarIns = GetQuickBarIns(QuickBarTag))
	{
		QuickBarIns->SetActiveSlotIndex(NewIndex);
	}
}

int32 UCancerQuickBarComponent::GetActiveSlotIndex(const FGameplayTag& QuickBarTag)
{
	if (auto QuickBarIns = GetQuickBarIns(QuickBarTag))
	{
		return QuickBarIns->GetActiveSlotIndex();
	}
	return -1;
}

UInventoryItemInstance* UCancerQuickBarComponent::GetActiveSlotItem(const FGameplayTag& QuickBarTag)
{
	if (auto QuickBarIns = GetQuickBarIns(QuickBarTag))
	{
		return QuickBarIns->GetActiveSlotItem();
	}
	return nullptr;
}

int32 UCancerQuickBarComponent::GetSlotCount(const FGameplayTag& QuickBarTag)
{
	if (auto QuickBarIns = GetQuickBarIns(QuickBarTag))
	{
		return QuickBarIns->GetValidSlots();
	}
	return -1;
}

bool UCancerQuickBarComponent::SetActiveSlotItem(const FGameplayTag& QuickBarTag, const FGameplayTag& ItemTag)
{
	if (auto QuickBarIns = GetQuickBarIns(QuickBarTag))
	{
		return QuickBarIns->SetActiveSlotItem(ItemTag);
	}
	return false;
}
