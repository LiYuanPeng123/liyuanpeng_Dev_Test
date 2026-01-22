#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CancerQuickBarComponent.generated.h"


class UInventoryItemInstance;
class UCancerQuickBarInstance;

UCLASS(ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent))
class CANCERINVENTORY_API UCancerQuickBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCancerQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void RestQuickBarIns();

public:
	UPROPERTY(Instanced, EditAnywhere, BlueprintReadOnly)
	TArray<UCancerQuickBarInstance*> QuickBarInstances;

	UFUNCTION(BlueprintCallable)
	void AddQuickBarInstance(UCancerQuickBarInstance* QuickBarIns);

	UFUNCTION(BlueprintCallable, meta=(GameplatTagFilter = "快捷栏"))
	UCancerQuickBarInstance* GetQuickBarIns(const FGameplayTag& QuickBarTag);

	//添加
	UFUNCTION(BlueprintCallable, meta=(GameplatTagFilter = "快捷栏"))
	void AddItemToSlot(const FGameplayTag& QuickBarTag, int32 SlotIndex, UInventoryItemInstance* Item);
	UFUNCTION(BlueprintCallable, meta=(GameplatTagFilter = "快捷栏"))
	void AddItem(const FGameplayTag& QuickBarTag, UInventoryItemInstance* Item);

	//移除
	UFUNCTION(BlueprintCallable)
	UInventoryItemInstance* RemoveItemFromSlot(const FGameplayTag& QuickBarTag, int32 SlotIndex);
	UFUNCTION(BlueprintCallable)
	UInventoryItemInstance* RemoveItemFromItemTag(const FGameplayTag& QuickBarTag, const FGameplayTag& ItemTag);

	// 清除武器快捷栏 在动画蓝图中会报错 
	UFUNCTION(BlueprintCallable)
	void ClearWeaponInstance();


	// 向前轮换激活槽位
	UFUNCTION(BlueprintCallable, Category="", meta=(GameplatTagFilter = "快捷栏"))
	bool CycleActiveSlotForward(const FGameplayTag& QuickBarTag);

	// 向后轮换激活槽位
	UFUNCTION(BlueprintCallable, Category="", meta=(GameplatTagFilter = "快捷栏"))
	bool CycleActiveSlotBackward(const FGameplayTag& QuickBarTag);

	// 设置当前激活槽位索引
	UFUNCTION(BlueprintCallable, Category="", meta=(GameplatTagFilter = "快捷栏"))
	void SetActiveSlotIndex(const FGameplayTag& QuickBarTag, int32 NewIndex);

	//获取激活索引
	UFUNCTION(BlueprintCallable, Category="", meta=(GameplatTagFilter = "快捷栏"))
	int32 GetActiveSlotIndex(const FGameplayTag& QuickBarTag);

	//获取激活道具
	UFUNCTION(BlueprintCallable, Category="", meta=(GameplatTagFilter = "快捷栏"))
	UInventoryItemInstance* GetActiveSlotItem(const FGameplayTag& QuickBarTag);

	// 获取快捷栏中 的武器数量 如果只有一个空手 AI需要尝试从背包中将武器放入快捷栏
	UFUNCTION(BlueprintCallable, Category="", meta=(GameplatTagFilter = "快捷栏"))
	int32 GetSlotCount(const FGameplayTag& QuickBarTag);

	UFUNCTION(BlueprintCallable, Category="")
	bool SetActiveSlotItem(const FGameplayTag& QuickBarTag, const FGameplayTag& ItemTag);
};
