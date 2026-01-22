#include "CancerInventoryFunctionLibrary.h"

#include "CancerInventorySubsystem.h"
#include "CancerNativeGamePlayTag.h"
#include "CancerQuickBarInstance.h"
#include "InventoryItemInstance.h"
#include "Components/CancerEquipmentManagerComponent.h"
#include "Components/CancerInventoryComponent.h"
#include "Components/CancerQuickBarComponent.h"
#include "GameFramework/CancerPickupActor.h"
#include "Interfaces/ICancerInteraction.h"
#include "QuickBarIns/QuickBarIns_Magic.h"
#include "QuickBarIns/QuickBarIns_Medicine.h"
#include "QuickBarIns/QuickBarIns_MilitaryClassics.h"
#include "QuickBarIns/QuickBarIns_Skills.h"
#include "QuickBarIns/QuickBarIns_Weapon.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"



const UInventoryItemFragment* UCancerInventoryFunctionLibrary::FindItemDefinitionFragment(
	TSubclassOf<UInventoryItemDefinition> ItemDef, TSubclassOf<UInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

UMeshComponent* UCancerInventoryFunctionLibrary::GetInteractionMesh(AActor* Actor)
{
	if (Actor->Implements<UICancerInteraction>())
	{
		return IICancerInteraction::Execute_GetInteractionMesh(Actor);
	}
	return nullptr;
}

UInventoryItemInstance* UCancerInventoryFunctionLibrary::AddItem(AActor* Actor, const FGameplayTag& ItemTag,
                                                                 bool& bIsValid, int32 Stack)
{
	bIsValid = false;
	if (auto InventoryComponent = GetCancerInventoryComponent(Actor))
	{
		UInventoryItemInstance* ItemInstance;
		bIsValid = InventoryComponent->AddItemAndResult(ItemTag, ItemInstance, Stack);
		return ItemInstance;
	}
	return nullptr;
}


class UCancerQuickBarInstance* UCancerInventoryFunctionLibrary::K2_GetCancerQuickBarInstance(AActor* Actor,
	const FGameplayTag& InTag, bool& bIsValid, TSubclassOf<UCancerQuickBarInstance> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetQuickBarIns(Actor, InTag))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerQuickBarInstance::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

class UCancerQuickBarInstance* UCancerInventoryFunctionLibrary::K2_GetCancerQuickBar_Weapon(AActor* Actor,
	bool& bIsValid, TSubclassOf<UQuickBarIns_Weapon> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetQuickBarIns(Actor, QuickBar::QuickBar_Weapon))
	{
		if (!OutputClass)
		{
			OutputClass = UQuickBarIns_Weapon::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}


class UCancerQuickBarInstance* UCancerInventoryFunctionLibrary::K2_GetCancerQuickBar_Skills(AActor* Actor,
	bool& bIsValid, TSubclassOf<UQuickBarIns_Skills> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetQuickBarIns(Actor, QuickBar::QuickBar_Skill))
	{
		if (!OutputClass)
		{
			OutputClass = UQuickBarIns_Skills::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

class UCancerQuickBarInstance* UCancerInventoryFunctionLibrary::K2_GetCancerQuickBar_Magic(AActor* Actor,
	bool& bIsValid, TSubclassOf<UQuickBarIns_Magic> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetQuickBarIns(Actor, QuickBar::QuickBar_Magic))
	{
		if (!OutputClass)
		{
			OutputClass = UQuickBarIns_Magic::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

class UCancerQuickBarInstance* UCancerInventoryFunctionLibrary::K2_GetCancerQuickBar_Medicine(AActor* Actor,
	bool& bIsValid, TSubclassOf<UQuickBarIns_Medicine> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetQuickBarIns(Actor, QuickBar::QuickBar_Medicine))
	{
		if (!OutputClass)
		{
			OutputClass = UQuickBarIns_Medicine::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

class UCancerQuickBarInstance* UCancerInventoryFunctionLibrary::K2_GetCancerQuickBar_MilitaryClassics(AActor* Actor,
	bool& bIsValid, TSubclassOf<UQuickBarIns_MilitaryClassics> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetQuickBarIns(Actor, QuickBar::QuickBar_MilitaryClassics))
	{
		if (!OutputClass)
		{
			OutputClass = UQuickBarIns_MilitaryClassics::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}


class ACancerPickupActor* UCancerInventoryFunctionLibrary::K2_SpawnItem(UObject* WorldContextObject,
                                                                        const FGameplayTag& ItemTag,
                                                                        const FTransform& Transform, bool& bIsValid,
                                                                        TSubclassOf<ACancerPickupActor> OutputClass)
{
	bIsValid = false;
	if (!WorldContextObject) return nullptr;
	if (auto InventorySubsystem = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<
		UCancerInventorySubsystem>())
	{
		if (!OutputClass)
			OutputClass = ACancerPickupActor::StaticClass();
		if (auto ItemDef = InventorySubsystem->FindInventoryDef(ItemTag))
		{
			if (ACancerPickupActor* NewItem = Cast<ACancerPickupActor>(
				WorldContextObject->GetWorld()->SpawnActor(OutputClass, &Transform)))
			{
				if (NewItem->Implements<UCancerPickupable>())
				{
					if (auto ItemInteractionDef = InventorySubsystem->FindInventoryPickupDef(ItemTag))
					{
						ICancerPickupable::Execute_SetInteractionDef(NewItem, ItemInteractionDef);
						ICancerPickupable::Execute_SetPickupItemTag(NewItem, ItemTag);
					}
				}
				bIsValid = NewItem->GetClass()->IsChildOf(OutputClass);
				return NewItem;
			}
		}
	}
	return nullptr;
}


class UCancerQuickBarComponent* UCancerInventoryFunctionLibrary::K2_GetCancerQuickBarComponent(
	AActor* Actor, bool& bIsValid,
	TSubclassOf<UCancerQuickBarComponent> OutputClass)
{
	bIsValid = false;

	if (auto ReturnValue = GetCancerQuickBarComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerQuickBarComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

class UCancerEquipmentManagerComponent* UCancerInventoryFunctionLibrary::K2_GetCancerEquipmentManagerComponent(
	AActor* Actor,
	bool& bIsValid, TSubclassOf<UCancerEquipmentManagerComponent> OutputClass)
{
	bIsValid = false;

	if (auto ReturnValue = GetCancerEquipmentManagerComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerEquipmentManagerComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}


class UCancerInventoryComponent* UCancerInventoryFunctionLibrary::K2_GetCancerInventoryComponent(AActor* Actor,
	bool& bIsValid, TSubclassOf<UCancerInventoryComponent> OutputClass)
{
	bIsValid = false;

	if (auto ReturnValue = GetCancerInventoryComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerInventoryComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}


class UInventoryItemInstance* UCancerInventoryFunctionLibrary::GetEquipWeaponItemInstance(AActor* Actor, bool& bIsValid)
{
	bIsValid = false;
	if (auto QuickBarComponent = GetCancerQuickBarComponent(Actor))
	{
		if (auto ItemIns = QuickBarComponent->GetActiveSlotItem(QuickBar::QuickBar_Weapon))
		{
			bIsValid = true;
			return ItemIns;
		}
	}
	return nullptr;
}


class UEquipmentInstance* UCancerInventoryFunctionLibrary::K2_GetEquipWeaponInstance(AActor* Actor, bool& bIsValid)
{
	bIsValid = false;
	if (auto QuickBarComponent = GetCancerQuickBarComponent(Actor))
	{
		if (auto QuickIns = GetQuickBarIns<UQuickBarIns_Weapon>(Actor, QuickBar::QuickBar_Weapon))
		{
			bIsValid = true;
			return QuickIns->GetEquipItem();
		}
	}
	return nullptr;
}


EEquipSocket UCancerInventoryFunctionLibrary::K2_GetEquipWeaponSocket(AActor* Actor, bool& bIsValid)
{
	bIsValid = false;
	if (auto QuickBarComponent = GetCancerQuickBarComponent(Actor))
	{
		if (auto QuickIns = GetQuickBarIns<UQuickBarIns_Weapon>(Actor, QuickBar::QuickBar_Weapon))
		{
			if (auto EquipIns = QuickIns->GetEquipItem())
			{
				bIsValid = true;
				return EquipIns->GetCurrentEquipSocket();
			}
		}
	}
	return EEquipSocket::Default;
}


class UInventoryItemInstance* UCancerInventoryFunctionLibrary::K2_GetEquipMagicInstance(AActor* Actor, bool& bIsValid)
{
	bIsValid = false;
	if (auto QuickBarComponent = GetCancerQuickBarComponent(Actor))
	{
		if (auto ItemIns = QuickBarComponent->GetActiveSlotItem(QuickBar::QuickBar_Magic))
		{
			bIsValid = true;
			return ItemIns;
		}
	}
	return nullptr;
}

class UInventoryItemInstance* UCancerInventoryFunctionLibrary::K2_GetEquipMedicineInstance(AActor* Actor,
	bool& bIsValid)
{
	bIsValid = false;
	if (auto QuickBarComponent = GetCancerQuickBarComponent(Actor))
	{
		if (auto ItemIns = QuickBarComponent->GetActiveSlotItem(QuickBar::QuickBar_Medicine))
		{
			bIsValid = true;
			return ItemIns;
		}
	}
	return nullptr;
}

class UInventoryItemInstance* UCancerInventoryFunctionLibrary::K2_GetEquipSkillInstance(AActor* Actor, bool& bIsValid)
{
	bIsValid = false;
	if (auto QuickBarComponent = GetCancerQuickBarComponent(Actor))
	{
		if (auto ItemIns = QuickBarComponent->GetActiveSlotItem(QuickBar::QuickBar_Skill))
		{
			bIsValid = true;
			return ItemIns;
		}
	}
	return nullptr;
}

class UInventoryItemInstance* UCancerInventoryFunctionLibrary::K2_GetEquipMilitaryClassicsInstance(AActor* Actor,
	bool& bIsValid)
{
	bIsValid = false;
	if (auto QuickBarComponent = GetCancerQuickBarComponent(Actor))
	{
		if (auto ItemIns = QuickBarComponent->GetActiveSlotItem(QuickBar::QuickBar_MilitaryClassics))
		{
			bIsValid = true;
			return ItemIns;
		}
	}
	return nullptr;
}
