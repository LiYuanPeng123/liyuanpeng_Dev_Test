#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/CancerQuickBarComponent.h"
#include "Equipment/EquipmentDefinition.h"
#include "Interfaces/ICancerPickupable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerInventoryFunctionLibrary.generated.h"


class UQuickBarIns_MilitaryClassics;
class UQuickBarIns_Medicine;
class UQuickBarIns_Magic;
class UQuickBarIns_Skills;
class UQuickBarIns_Weapon;
class UCancerQuickBarInstance;
class ACancerPickupActor;
class UCancerEquipmentManagerComponent;
class UCancerQuickBarComponent;
class UInventoryItemFragment;
class UInventoryItemDefinition;
class UCancerInventoryComponent;
class UInventoryComponent;

UCLASS()
class CANCERINVENTORY_API UCancerInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	

	//查找装备的装备片段
	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UInventoryItemDefinition> ItemDef,
	                                                                TSubclassOf<UInventoryItemFragment> FragmentClass);

	//获取交互网格体
	UFUNCTION(BlueprintCallable)
	static UMeshComponent* GetInteractionMesh(AActor* Actor);

#pragma region 背包道具
	UFUNCTION(BlueprintCallable, Category = "Inventory",
		meta=(ExpandBoolAsExecs="bIsValid"))
	static UInventoryItemInstance* AddItem(AActor* Actor,UPARAM(meta = (Categories = "道具"))
	                                       const FGameplayTag& ItemTag, bool& bIsValid, int32 Stack = 1);
#pragma endregion

#pragma  region //获取指定背包组件
	template <class T = UCancerInventoryComponent>
	static T* GetCancerInventoryComponent(AActor* Actor)
	{
		if (!Actor) return nullptr;

		return Actor->GetComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetCancerInventoryComponent",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerInventoryComponent* K2_GetCancerInventoryComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerInventoryComponent> OutputClass = nullptr);

#pragma endregion

#pragma  region //获取装备组件
	template <class T = UCancerEquipmentManagerComponent>
	static T* GetCancerEquipmentManagerComponent(AActor* Actor)
	{
		if (!Actor) return nullptr;

		return Actor->GetComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetCancerEquipmentManagerComponent",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerEquipmentManagerComponent* K2_GetCancerEquipmentManagerComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerEquipmentManagerComponent> OutputClass = nullptr);

#pragma endregion

#pragma  region //获取装备插槽组件
	template <class T = UCancerQuickBarComponent>
	static T* GetCancerQuickBarComponent(AActor* Actor)
	{
		if (!Actor) return nullptr;

		return Actor->GetComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="Get CancerQuickBarComponent",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerQuickBarComponent* K2_GetCancerQuickBarComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerQuickBarComponent> OutputClass = nullptr);
#pragma endregion

#pragma  region // 获取指定快捷栏组件
	template <class T = UCancerQuickBarInstance>
	static T* GetQuickBarIns(AActor* Actor, const FGameplayTag& InTag)
	{
		if (!Actor) return nullptr;
		auto Component = GetCancerQuickBarComponent(Actor);
		if (Component == nullptr) return nullptr;

		return Cast<T>(Component->GetQuickBarIns(InTag));
	}

	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetCancerQuickBarInstance",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerQuickBarInstance* K2_GetCancerQuickBarInstance(
		AActor* Actor, const FGameplayTag& InTag, bool& bIsValid,
		TSubclassOf<UCancerQuickBarInstance> OutputClass = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetCancerQuickBar_Weapon",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerQuickBarInstance* K2_GetCancerQuickBar_Weapon(
		AActor* Actor, bool& bIsValid,
		TSubclassOf<UQuickBarIns_Weapon> OutputClass = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetCancerQuickBar_Skills",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerQuickBarInstance* K2_GetCancerQuickBar_Skills(
		AActor* Actor, bool& bIsValid,
		TSubclassOf<UQuickBarIns_Skills> OutputClass = nullptr);


	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetCancerQuickBar_Magic",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerQuickBarInstance* K2_GetCancerQuickBar_Magic(
		AActor* Actor, bool& bIsValid,
		TSubclassOf<UQuickBarIns_Magic> OutputClass = nullptr);


	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetCancerQuickBar_Medicine",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerQuickBarInstance* K2_GetCancerQuickBar_Medicine(
		AActor* Actor, bool& bIsValid,
		TSubclassOf<UQuickBarIns_Medicine> OutputClass = nullptr);


	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetCancerQuickBar_MilitaryClassics",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerQuickBarInstance* K2_GetCancerQuickBar_MilitaryClassics(
		AActor* Actor, bool& bIsValid,
		TSubclassOf<UQuickBarIns_MilitaryClassics> OutputClass = nullptr);

#pragma endregion

#pragma  region //生成道具
	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="SapwnItem",
		meta=(WorldContext = "WorldContextObject", DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class ACancerPickupActor* K2_SpawnItem(
		UObject* WorldContextObject,UPARAM(meta = (Categories = "道具"))
		const FGameplayTag& ItemTag, const FTransform& Transform, bool& bIsValid,
		TSubclassOf<ACancerPickupActor> OutputClass = nullptr);
#pragma endregion

#pragma  region //获取激活的武器道具实例
	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetEquipWeaponItemInstance",
		meta=(ExpandBoolAsExecs="bIsValid"))
	static class UInventoryItemInstance* GetEquipWeaponItemInstance(
		AActor* Actor, bool& bIsValid);
#pragma endregion
#pragma  region //获取激活的武器装备实例
	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetEquipWeaponInstance",
		meta=(ExpandBoolAsExecs="bIsValid"))
	static class UEquipmentInstance* K2_GetEquipWeaponInstance(
		AActor* Actor, bool& bIsValid);
#pragma endregion

#pragma  region //获取激活的武器插槽状态
	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetEquipWeaponSocket",
		meta=(ExpandBoolAsExecs="bIsValid"))
	static EEquipSocket K2_GetEquipWeaponSocket(
		AActor* Actor, bool& bIsValid);
#pragma endregion

#pragma  region //获取激活的法宝实例
	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetEquipMagicInstance",
		meta=(ExpandBoolAsExecs="bIsValid"))
	static class UInventoryItemInstance* K2_GetEquipMagicInstance(
		AActor* Actor, bool& bIsValid);
#pragma endregion

#pragma  region //获取激活的丹药实例
	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetEquipMedicineInstance",
		meta=(ExpandBoolAsExecs="bIsValid"))
	static class UInventoryItemInstance* K2_GetEquipMedicineInstance(
		AActor* Actor, bool& bIsValid);
#pragma endregion

#pragma  region //获取激活的武技实例
	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetEquipSkillInstance",
		meta=(ExpandBoolAsExecs="bIsValid"))
	static class UInventoryItemInstance* K2_GetEquipSkillInstance(
		AActor* Actor, bool& bIsValid);
#pragma endregion

#pragma  region //获取激活的功法实例
	UFUNCTION(BlueprintCallable, Category = "Inventory", DisplayName="GetEquipMilitaryClassicsInstance",
		meta=(ExpandBoolAsExecs="bIsValid"))
	static class UInventoryItemInstance* K2_GetEquipMilitaryClassicsInstance(
		AActor* Actor, bool& bIsValid);
#pragma endregion
};
