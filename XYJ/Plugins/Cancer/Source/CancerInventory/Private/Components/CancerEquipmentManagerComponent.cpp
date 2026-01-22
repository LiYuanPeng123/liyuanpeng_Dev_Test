#include "Components/CancerEquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentInstance.h"

// ============================================================================
//  装备管理组件实现文件
//  职责：
//  - FAppliedEquipmentEntry：记录已装备的信息（定义 + 实例 + 句柄）。
//  - FEquipmentList：内部容器，负责创建/移除装备实例，并管理其生命周期关联的 Actor。
//  - UEquipmentManagerComponent：对外提供 Equip / Unequip / 查询 等接口；
//                               在组件反初始化时，确保所有装备被正确卸载。
//  说明：以下仅添加中文注释，不修改任何现有逻辑或标识符。
// ============================================================================


FString FAppliedEquipmentEntry::GetDebugString() const
{
	// 打印调试信息："实例名 of 定义类名"
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

UEquipmentInstance* FEquipmentList::AddEntry(TSubclassOf<UEquipmentDefinition> EquipmentDefinition)
{
	// 功能：根据装备定义创建一条装备记录，并返回创建出来的装备实例
	UEquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);

	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<UEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UEquipmentInstance::StaticClass();
	}

	FAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);

	Result = NewEntry.Instance;

	//装备附加技能
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		// 注意：GiveToAbilitySystem 为非 const 成员函数，这里需要非 const 指针类型
		for (const TObjectPtr<UCancerAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			if (AbilitySet)
				AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		//@TODO: Warning logging?
	}

	//生成装备
	// 根据定义配置，在 Pawn 上生成与该装备相关的附属 Actor（如武器、配件、特效载体等）
	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);
	return Result;
}

void FEquipmentList::RemoveEntry(UEquipmentInstance* Instance)
{
	// 功能：移除一条已装备记录；
	// 流程：若启用能力系统集成，先移除授予的能力；随后销毁由装备生成的附属 Actor；最后移出列表。
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			//移除装备所赋予的技能
			if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			// 销毁由装备实例生成的所有附属 Actor
			Instance->DestroyEquipmentActors();

			// 从列表中删除该条记录
			EntryIt.RemoveCurrent();
		}
	}
}

UAbilitySystemComponent* FEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerComponent->GetOwner());
}

UCancerEquipmentManagerComponent::UCancerEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , EquipmentList(this)
{
	// 参与 InitializeComponent/UninitializeComponent 生命周期，便于在组件卸载时清理装备
	bWantsInitializeComponent = true;
}

UEquipmentInstance* UCancerEquipmentManagerComponent::EquipItem(TSubclassOf<UEquipmentDefinition> EquipmentClass)
{
	// 对外穿戴接口：内部委托给 FEquipmentList 创建实例，并调用实例的 OnEquipped 回调
	UEquipmentInstance* Result = nullptr;
	if (EquipmentClass != nullptr)
	{
		Result = EquipmentList.AddEntry(EquipmentClass);
		if (Result != nullptr)
		{
			Result->OnEquipped();
		}
	}
	return Result;
}

void UCancerEquipmentManagerComponent::UnequipItem(UEquipmentInstance* ItemInstance)
{
	// 对外卸下接口：调用实例 OnUnequipped 回调，并让 FEquipmentList 执行移除逻辑
	if (ItemInstance != nullptr)
	{
		ItemInstance->OnUnequipped();
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

void UCancerEquipmentManagerComponent::InitializeComponent()
{
	// 组件初始化：当前无额外逻辑，保留扩展点
	Super::InitializeComponent();
}

void UCancerEquipmentManagerComponent::UninitializeComponent()
{
	// 组件反初始化：统一卸载所有已装备实例，避免迭代过程中修改容器带来的副作用
	TArray<UEquipmentInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator 	
	for (const FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (UEquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}


UEquipmentInstance* UCancerEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UEquipmentInstance> InstanceType)
{
	// 查询第一个匹配指定类型的装备实例
	for (FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}
	return nullptr;
}

TArray<UEquipmentInstance*> UCancerEquipmentManagerComponent::GetEquipmentInstancesOfType(
	TSubclassOf<UEquipmentInstance> InstanceType) const
{
	// 查询所有匹配指定类型的装备实例
	TArray<UEquipmentInstance*> Results;
	for (const FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}
