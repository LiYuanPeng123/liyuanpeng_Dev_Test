#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory_Item.generated.h"

class UInventory_ItemContainer;
class UImage;
class UTextBlock;
class UButton;
class UInventoryItemInstance;
/**
 * 
 */
UCLASS()
class CANCERINVENTORY_API UInventory_Item : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	//初始化
	UFUNCTION(BlueprintCallable)
	void OnItemInitialized(UInventory_ItemContainer* InContainer, int32 InItemIndex);

	//更新
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnItemUpdated(UInventoryItemInstance* Item);

	//选择
	UFUNCTION(BlueprintCallable)
	void SetSelected(bool bSelected);

	//重置
	UFUNCTION(BlueprintCallable)
	void ItemReset();

protected:
	UPROPERTY(BlueprintReadWrite)
	UInventoryItemInstance* CacheItemInstance;

	UPROPERTY(BlueprintReadWrite)
	int32 ItemIndex;

	UPROPERTY(BlueprintReadWrite)
	UInventory_ItemContainer* Container;

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true, BindWidget))
	UButton* ItemButton;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true, BindWidget))
	UTextBlock* ItemNumber;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true, BindWidget))
	UImage* Image_Selected;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true, BindWidget))
	UImage* Image_Item;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true, BindWidget))
	UImage* Image_Back;

	UFUNCTION()
	void OnItemButtonClicked();
	UFUNCTION()
	void OnItemHovered();
};
