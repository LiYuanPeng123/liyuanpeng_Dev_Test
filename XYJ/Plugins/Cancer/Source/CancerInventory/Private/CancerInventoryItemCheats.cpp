// Fill out your copyright notice in the Description page of Project Settings.


#include "CancerInventoryItemCheats.h"

#include "CancerNativeGamePlayTag.h"
#include "Components/CancerInventoryComponent.h"
#include "Components/CancerQuickBarComponent.h"

UCancerInventoryItemCheats::UCancerInventoryItemCheats()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UCheatManager::RegisterForOnCheatManagerCreated(FOnCheatManagerCreated::FDelegate::CreateLambda(
			[](UCheatManager* CheatManager)
			{
				CheatManager->AddCheatManagerExtension(NewObject<ThisClass>(CheatManager));
			}));
	}
#endif
}

void UCancerInventoryItemCheats::AddItem(int32 ItemID, int32 Count)
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (UCancerInventoryComponent* InventoryManager = GetInventoryComponent())
	{
		InventoryManager->AddItemByID(ItemID, Count);
	}
#endif
}

void UCancerInventoryItemCheats::RemoveItem(int32 ItemID, int32 Count)
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (UCancerInventoryComponent* InventoryManager = GetInventoryComponent())
	{
		InventoryManager->RemoveItemByID(ItemID, Count);
	}
#endif
}

void UCancerInventoryItemCheats::ClearQuickBarItem()
{
	if (UWorld* World = GetWorld())
	{
		if (auto Pawn = World->GetFirstPlayerController()->GetPawn())
		{
			if (UCancerQuickBarComponent* QuickBarComponent =  Pawn->FindComponentByClass<UCancerQuickBarComponent>())
			{
				QuickBarComponent->ClearWeaponInstance();
			}
		}
	}
}

void UCancerInventoryItemCheats::MoveWeaponItemToQuickBar()
{
	if (UCancerInventoryComponent* InventoryManager = GetInventoryComponent())
	{
		TArray<UInventoryItemInstance*> AllWeapon = InventoryManager->FindItemsByTagMatch(FGameplayTag::RequestGameplayTag(TEXT("道具.武器")));

		if (AllWeapon.IsEmpty()) return;
		
		if (UWorld* World = GetWorld())
		{
			if (auto Pawn = World->GetFirstPlayerController()->GetPawn())
			{
				if (UCancerQuickBarComponent* QuiclBarComponent =  Pawn->FindComponentByClass<UCancerQuickBarComponent>())
				{
					for (auto WeaponInstance : AllWeapon)
					{
						QuiclBarComponent->AddItem(QuickBar::QuickBar_Weapon,WeaponInstance);
					}
				}
			}
		}
	}
}

UCancerInventoryComponent* UCancerInventoryItemCheats::GetInventoryComponent() const
{
	if (UWorld* World = GetWorld())
	{
		if (auto Pawn = World->GetFirstPlayerController()->GetPawn())
		{
			return Pawn->FindComponentByClass<UCancerInventoryComponent>();
		}
	}

	return nullptr;
}
