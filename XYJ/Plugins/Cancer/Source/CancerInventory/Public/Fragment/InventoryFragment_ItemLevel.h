#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDefinition.h"
#include "InventoryFragment_ItemLevel.generated.h"

/**
 * 物品等级片段
 * 用于定义物品的等级相关属性
 */
UCLASS(DisplayName="物品等级")
class CANCERINVENTORY_API UInventoryFragment_ItemLevel : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UInventoryFragment_ItemLevel()
	{
		LevelDisplays.Add({1,TEXT("良品")});
		LevelDisplays.Add({2,TEXT("上品")});
		LevelDisplays.Add({3,TEXT("特品")});
		LevelDisplays.Add({4,TEXT("仙品")});
		LevelDisplays.Add({5,TEXT("神珍")});
	}
	// 物品的基础等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|ItemLevel", meta = (ClampMin = "1", UIMin = "1"))
	int32 BaseLevel = 1;

	// 物品的最大等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|ItemLevel", meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|ItemLevel")
	TMap<int32, FName> LevelDisplays;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|ItemLevel")
	virtual FName GetItemLevelDisplay() const { return LevelDisplays[BaseLevel]; }
	
	// 获取物品等级（可以被子类重写以实现自定义等级逻辑）b
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|ItemLevel")
	virtual int32 GetItemLevel() const { return BaseLevel; }

	// 检查是否可以升级
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|ItemLevel")
	virtual bool CanLevelUp() const { return BaseLevel < MaxLevel; }

	// 升级物品（返回是否升级成功）
	UFUNCTION(BlueprintCallable, Category = "Inventory|ItemLevel")
	virtual bool LevelUp()
	{
		if (CanLevelUp())
		{
			BaseLevel++;
			return true;
		}
		return false;
	}
};
