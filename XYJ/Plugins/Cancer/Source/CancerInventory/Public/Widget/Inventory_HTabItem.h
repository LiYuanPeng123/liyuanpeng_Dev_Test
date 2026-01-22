#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory_HTabItem.generated.h"

class UImage;
class UTextBlock;
class UHTabDataAsset;
class UInventory_HTab;
class UButton;
/**
 * 
 */
UCLASS()
class CANCERINVENTORY_API UInventory_HTabItem : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable)
	void OnButtonClicked();

	UFUNCTION(BlueprintCallable)
	void SetSelected(bool bSelected);

	UFUNCTION(BlueprintCallable)
	void TabItemInit(const FName& InTabName, TSoftObjectPtr<UTexture2D> InImage,
	                 TSoftObjectPtr<UTexture2D> SelectedImage, int32 InTabIndex, UInventory_HTab* OwnerHTab);

	UPROPERTY(BlueprintReadWrite)
	FName TabName;
	UPROPERTY(BlueprintReadWrite)
	int32 TabIndex;

	UPROPERTY(BlueprintReadWrite)
	UInventory_HTab* CacheHTab;

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true, BindWidget))
	UButton* TabButton;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true, BindWidget))
	UTextBlock* TabNameText;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true, BindWidget))
	UImage* Image_Selected;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true, BindWidget))
	UImage* Image_Item;
};
