#pragma once

#include "CoreMinimal.h"
#include "CancerInventoryFunctionLibrary.h"
#include "CancerQuickBarInstance.h"
#include "Equipment/EquipmentInstance.h"
#include "QuickBarIns_Weapon.generated.h"

/**
 * 
 */
UCLASS(DisplayName="武器")
class CANCERINVENTORY_API UQuickBarIns_Weapon : public UCancerQuickBarInstance
{
	GENERATED_BODY()

public:
	UQuickBarIns_Weapon();
	virtual void EquipItemInSlot_Implementation() override;
	virtual void UnequipItemInSlot_Implementation() override;

	UFUNCTION(BlueprintCallable, Category=Equipment)
	void AttachEquipToSlot(EEquipSocket InEquipSocket);

	virtual ACancerPickupActor* DiscardItemInSlot_Implementation(const FTransform& Transform) override;
	
	class UCancerEquipmentManagerComponent* FindEquipmentManager() const;

	UFUNCTION(BlueprintCallable, Category=Equipment)
	UEquipmentInstance* GetEquipItem()
	{
		return EquippedItem;
	};

	UFUNCTION(BlueprintCallable, Category=Equipment)
	EEquipSocket GetCurrentEquipItemSocket();

protected:
	// 当前已装备的装备实例（由激活槽位驱动）
	UPROPERTY()
	TObjectPtr<UEquipmentInstance> EquippedItem;
};
