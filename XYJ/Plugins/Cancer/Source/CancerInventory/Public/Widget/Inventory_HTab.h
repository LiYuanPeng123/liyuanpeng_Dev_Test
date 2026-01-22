#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/ItemTabInfoObject.h"
#include "Inventory_HTab.generated.h"

class UInventory_HTabItem;
class UHorizontalBox;


UCLASS()
class CANCERINVENTORY_API UInventory_HTab : public UUserWidget
{
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTabChangedDelegate, UInventory_HTabItem*, InTableItem);

public:
	UPROPERTY(BlueprintAssignable)
	FTabChangedDelegate TabChangedDelegate;
	
	UFUNCTION(BlueprintCallable)
	void SetSelectedTab(int32 InTabIndex = 0);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AdvanceToNextOption();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BackToPreviousOption();

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnInitializedHTab();
	
	UFUNCTION(BlueprintCallable)
	const FName& GetActiveTabName();

	UFUNCTION(BlueprintPure)
	UItemTabInfoObject* GetTabInfoByTableName(const FName& InTabName);

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<UInventory_HTabItem*> InventoryItems;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<FName,UItemTabInfoObject*> TableItemInfoMap;
	
	UPROPERTY(BlueprintReadWrite)
	int32 CurSelectedTab;
	UPROPERTY(BlueprintReadWrite)
	int32 LastSelectedTab;

private:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	UHorizontalBox* HorizontalBox;
};
