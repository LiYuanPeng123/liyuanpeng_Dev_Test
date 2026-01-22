#pragma once

#include "CoreMinimal.h"
#include "Data/CancerAbilitySet.h"
#include "CancerEquipmentManagerComponent.generated.h"
class UAbilitySystemComponent;
struct FGameplayTag;
struct FInventoryItemInfo;
struct FEquipmentList;
class UCancerEquipmentManagerComponent;
class UEquipmentDefinition;
class UEquipmentInstance;

/** 一件装备结构 */
USTRUCT(BlueprintType)
struct CANCERINVENTORY_API  FAppliedEquipmentEntry
{
	GENERATED_BODY()

	FAppliedEquipmentEntry()
	{
	}

	FString GetDebugString() const;

private:
	friend FEquipmentList;
	friend UCancerEquipmentManagerComponent;

	//装备定义
	UPROPERTY()
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

	//装备实例
	UPROPERTY()
	TObjectPtr<UEquipmentInstance> Instance = nullptr;

	// 装备赋予的能力
	UPROPERTY()
	FCancerAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct CANCERINVENTORY_API FEquipmentList
{
	GENERATED_BODY()

	FEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	UEquipmentInstance* AddEntry(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UEquipmentInstance* Instance);

private:
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UCancerEquipmentManagerComponent;

private:
	UPROPERTY()
	TArray<FAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

/*
装备管理组件（穿戴/脱卸）
*/
UCLASS(ClassGroup=(Custom), Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class CANCERINVENTORY_API UCancerEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCancerEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	//装备道具
	UFUNCTION(BlueprintCallable)
	UEquipmentInstance* EquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);
	
	
	//卸下道具
	UFUNCTION(BlueprintCallable)
	void UnequipItem(UEquipmentInstance* ItemInstance);

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	//~End of UActorComponent interface

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UEquipmentInstance> InstanceType);

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UEquipmentInstance> InstanceType) const;

private:
	UPROPERTY()
	FEquipmentList EquipmentList;
};
