
#pragma once

#include "CoreMinimal.h"
#include "CancerQuickBarInstance.h"
#include "Data/CancerAbilitySet.h"
#include "QuickBarIns_Magic.generated.h"

/**
 * 
 */
UCLASS(DisplayName="法宝")
class CANCERINVENTORY_API UQuickBarIns_Magic : public UCancerQuickBarInstance
{
	GENERATED_BODY()
public:
	UQuickBarIns_Magic();
	virtual void RestoreQuickBarIns() override;
	virtual void EquipItemInSlot_Implementation() override;
	virtual void UnequipItemInSlot_Implementation() override;
	UPROPERTY()
	FCancerAbilitySet_GrantedHandles GrantedHandle;
};
