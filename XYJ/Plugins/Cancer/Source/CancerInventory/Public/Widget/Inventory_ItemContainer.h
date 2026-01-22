
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/UIDefinition.h"
#include "Inventory_ItemContainer.generated.h"
class UScrollBox;
class UInventory_Item;
class UCancerInventoryComponent;
class UWrapBox;
/**
 * 
 */
UCLASS()
class CANCERINVENTORY_API UInventory_ItemContainer : public UUserWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSelected, UInventory_Item*, InventoryItem);
	public:
	
	UPROPERTY(BlueprintAssignable)
	FOnItemSelected OnItemSelected;
	
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnInitializedContainer(AActor*Actor,TSubclassOf<UInventory_Item> ItemUIClass,int32 ItemNumber);

	//初始化生成背包格子
	void OnInitializedInventoryContainer(AActor*Actor,TSubclassOf<UInventory_Item> ItemUIClass,int32 ItemNumber);
	
	
	//刷新背包格子
	UFUNCTION(BlueprintCallable)
	void OnItemUpdate(const FGameplayTag& MatchTag);

	//设置选择
	UFUNCTION(BlueprintCallable)
	void SetSelectedItem(int32 NewSelectedIndex);

	UFUNCTION(BlueprintCallable)
	UCancerInventoryComponent* GetInventoryComponent()const;
	
	UFUNCTION(BlueprintCallable)
	void WASDSelected(const EDirectionType& Direction);
	
	UFUNCTION(BlueprintCallable)
	void SetScrollBoxOffset(int32 NewScrollBoxOffset);
	
protected:
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UWrapBox* InventoryContainer;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UScrollBox* InventoryScrollBox;
	
	UPROPERTY(BlueprintReadWrite)
	AActor* OwnerActor;

	//生成的UI格子
	UPROPERTY(BlueprintReadWrite)
	TArray<UInventory_Item*>  InventoryUIList;
	
	UPROPERTY(BlueprintReadWrite)
	int32 LastItemIndex;
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentItemIndex;
};
