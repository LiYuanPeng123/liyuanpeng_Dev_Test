#include "Action_AddEquip.h"

#include "CancerInventoryFunctionLibrary.h"
#include "CancerNativeGamePlayTag.h"
#include "CancerQuickBarInstance.h"
#include "Components/CancerInventoryComponent.h"
#include "Components/CancerQuickBarComponent.h"
#include "QuickBarIns/QuickBarIns_Magic.h"
#include "QuickBarIns/QuickBarIns_Medicine.h"
#include "QuickBarIns/QuickBarIns_MilitaryClassics.h"
#include "QuickBarIns/QuickBarIns_Weapon.h"
#include "QuickBarIns/QuickBarIns_Skills.h"

UAction_AddEquip::UAction_AddEquip()

{
	QuickBarInstances.Add(CreateOptionalDefaultSubobject<UQuickBarIns_MilitaryClassics>(FName("QuickBarIns_MilitaryClassics")));
	QuickBarInstances.Add(CreateOptionalDefaultSubobject<UQuickBarIns_Skills>(FName("QuickBarIns_Skills")));
	QuickBarInstances.Add(CreateOptionalDefaultSubobject<UQuickBarIns_Medicine>(FName("QuickBarIns_Medicine")));
	QuickBarInstances.Add(CreateOptionalDefaultSubobject<UQuickBarIns_Magic>(FName("QuickBarIns_Magic")));
	QuickBarInstances.Add(CreateOptionalDefaultSubobject<UQuickBarIns_Weapon>(FName("QuickBarIns_Weapon")));
	
}

void UAction_AddEquip::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	Super::GatherSoftReferences(OutPaths);

	if (!ItemSetData.IsNull())
	{
		OutPaths.Add(ItemSetData.ToSoftObjectPath());
	}
	if (!EquipSetData.IsNull())
	{
		OutPaths.Add(EquipSetData.ToSoftObjectPath());
	}
}

void UAction_AddEquip::PostInitComponent_Implementation()
{
	
	
	UCancerQuickBarComponent* QuickBarComponent = UCancerInventoryFunctionLibrary::GetCancerQuickBarComponent(
		GetCreatActor());
	UCancerInventoryComponent* InventoryComponent = UCancerInventoryFunctionLibrary::GetCancerInventoryComponent(
		GetCreatActor());
	check(QuickBarComponent);
	check(InventoryComponent);

	//确保武器后初始化
	int32 LastIndex = QuickBarInstances.Num()-1;
	for (int32 i = 0; i <LastIndex ; ++i)
	{
		if (QuickBarInstances[i]->IsA(UQuickBarIns_Weapon::StaticClass()))
		{
			if (LastIndex != i)
			{
				QuickBarInstances.Swap(LastIndex, i);
			}
		}
	}
	
	for (auto Ins :QuickBarInstances)
	{
		UCancerQuickBarInstance* RuntimeIns = DuplicateObject<UCancerQuickBarInstance>(
											Ins, GetCreatActor());
		QuickBarComponent->AddQuickBarInstance(RuntimeIns);
	}
}

void UAction_AddEquip::FinalizeAfterComponent_Implementation()
{
	Super::FinalizeAfterComponent_Implementation();

	auto InventoryComponent = UCancerInventoryFunctionLibrary::GetCancerInventoryComponent(GetCreatActor());
	auto QuickBarComponent = UCancerInventoryFunctionLibrary::GetCancerQuickBarComponent(GetCreatActor());

	check(InventoryComponent);
	check(QuickBarComponent);
	TArray<FItemSet> ItemSets;
	if (GrantItemSource == ECancerGrantSource::DataTables)
	{
		ItemSets = GetData<FItemSet>(ItemSetData.Get());
	}
	else
	{
		ItemSets = ActivateItemSets;
	}
	TArray<FEquipSet> EquipSets ;
	if (GrantEquipSource == ECancerGrantSource::DataTables)
	{
		EquipSets = GetData<FEquipSet>(EquipSetData.Get());
	}
	else
	{
		EquipSets = ActivateEquipSets;
	}
	
	for (auto ItemSet : ItemSets)
	{
		InventoryComponent->AddItem(ItemSet.ItemTag, ItemSet.ItemNumber);
	}
	for (auto EquipSet : EquipSets)
	{
		InventoryComponent->AddItem(EquipSet.ItemTag, EquipSet.ItemNumber);
		auto Item = InventoryComponent->FindItem(EquipSet.ItemTag);
		QuickBarComponent->AddItemToSlot(EquipSet.QuickBarTag, EquipSet.SlotID, Item);
	}
	for (auto QuickIns : QuickBarComponent->QuickBarInstances)
	{
		QuickIns->SetActiveSlotIndex(SlotID);
	}
	if (auto QuickIns = UCancerInventoryFunctionLibrary::GetQuickBarIns<UQuickBarIns_Weapon>(
		GetCreatActor(), QuickBar::QuickBar_Weapon))
	{
		QuickIns->AttachEquipToSlot(EquipSocket);
	}
}
