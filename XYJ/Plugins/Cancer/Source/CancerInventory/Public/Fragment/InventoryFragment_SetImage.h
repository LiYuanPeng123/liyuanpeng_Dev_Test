
#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDefinition.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InventoryFragment_SetImage.generated.h"

UENUM(BlueprintType)
enum class EImageSourceType : uint8
{
	Direct UMETA(DisplayName = "直接配置"),     // 直接配置图片
	LookupKey  UMETA(DisplayName = "通过Key查找"),   // 通过Key从GameSubsystem中查找
};

/**
 * 物品显示图标配置
 * 支持直接配置图片或通过Key从GameSubsystem中查找
 */
UCLASS(DisplayName="物品显示图标")
class CANCERINVENTORY_API UInventoryFragment_SetImage : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	// 图片源类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Image")
	EImageSourceType ImageSourceType = EImageSourceType::Direct;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Image", meta = (EditCondition = "ImageSourceType == EImageSourceType::Direct", EditConditionHides))
	TSoftObjectPtr<UTexture2D> Image_Item;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Image", meta = (EditCondition = "ImageSourceType == EImageSourceType::LookupKey", EditConditionHides))
	int32 Image_ItemKey = -1;

	// 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Image", meta = (EditCondition = "ImageSourceType == EImageSourceType::Direct", EditConditionHides))
	TSoftObjectPtr<UTexture2D> Image_Selected_Back;

		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Image", meta = (EditCondition = "ImageSourceType == EImageSourceType::LookupKey", EditConditionHides))
	int32 Image_Selected_BackKey = -1;

public:
	// 获取物品图片
	UFUNCTION(BlueprintCallable,BlueprintPure, Category = "Inventory|Image", meta=(WorldContext="WorldContextObject"))
	TSoftObjectPtr<UTexture2D> GetItemImage(UObject* WorldContextObject = nullptr) const;

	// 获取选中状态背景图
	UFUNCTION(BlueprintCallable,BlueprintPure, Category = "Inventory|Image", meta=(WorldContext="WorldContextObject"))
	TSoftObjectPtr<UTexture2D> GetSelectedBackImage(UObject* WorldContextObject = nullptr) const;
};
