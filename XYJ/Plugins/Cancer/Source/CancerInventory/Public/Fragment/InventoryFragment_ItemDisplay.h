#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDefinition.h"
#include "InventoryFragment_ItemDisplay.generated.h"

UENUM(BlueprintType)
enum class EStrDisplaySourceType : uint8
{
	Direct UMETA(DisplayName = "直接配置"), // 直接配置图片
	LookupKey UMETA(DisplayName = "通过Key查找"), // 通过Key从GameSubsystem中查找
};

/**
 * 
 */
UCLASS(DisplayName="物品描述")
class CANCERINVENTORY_API UInventoryFragment_ItemDisplay : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EStrDisplaySourceType DisplaySourceType = EStrDisplaySourceType::LookupKey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "DisplaySourceType == EStrDisplaySourceType::Direct", EditConditionHides))
	FName ItemName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly
		, meta = (EditCondition = "DisplaySourceType == EStrDisplaySourceType::Direct", EditConditionHides))
	FName ItemDisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly
		, meta = (EditCondition = "DisplaySourceType == EStrDisplaySourceType::Direct", EditConditionHides))
	FName ItemName1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly
		, meta = (EditCondition = "DisplaySourceType == EStrDisplaySourceType::Direct", EditConditionHides))
	FName ItemName2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly
		, meta = (EditCondition = "DisplaySourceType == EStrDisplaySourceType::Direct", EditConditionHides))
	FName ItemName3;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly
		, meta = (EditCondition = "DisplaySourceType == EStrDisplaySourceType::LookupKey", EditConditionHides))
	int32 ItemNameIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly
		, meta = (EditCondition = "DisplaySourceType == EStrDisplaySourceType::LookupKey", EditConditionHides))
	int32 ItemDisplayNameIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly
		, meta = (EditCondition = "DisplaySourceType == EStrDisplaySourceType::LookupKey", EditConditionHides))
	int32 ItemNameIndex1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly
		, meta = (EditCondition = "DisplaySourceType == EStrDisplaySourceType::LookupKey", EditConditionHides))
	int32 ItemNameIndex2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly
		, meta = (EditCondition = "DisplaySourceType == EStrDisplaySourceType::LookupKey", EditConditionHides))
	int32 ItemNameIndex3;

	UFUNCTION(BlueprintCallable,BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	FName GetItemName(const UObject* WorldContextObject = nullptr);
	UFUNCTION(BlueprintCallable,BlueprintPure,meta = ( WorldContext = "WorldContextObject"))
	FName GetDisplayName(const UObject* WorldContextObject = nullptr);
	UFUNCTION(BlueprintCallable,BlueprintPure, meta = ( WorldContext = "WorldContextObject"))
	FName GetItemName1(const UObject* WorldContextObject = nullptr);
	UFUNCTION(BlueprintCallable,BlueprintPure, meta = ( WorldContext = "WorldContextObject"))
	FName GetItemName2(const UObject* WorldContextObject = nullptr);
	UFUNCTION(BlueprintCallable,BlueprintPure, meta = ( WorldContext = "WorldContextObject"))
	FName GetItemName3(const UObject* WorldContextObject = nullptr);
	
};
