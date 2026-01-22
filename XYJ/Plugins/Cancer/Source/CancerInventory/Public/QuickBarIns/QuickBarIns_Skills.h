#pragma once

#include "CoreMinimal.h"
#include "CancerQuickBarInstance.h"
#include "Data/CancerAbilitySet.h"
#include "QuickBarIns_Skills.generated.h"

/**
 * 
 */
UCLASS(DisplayName="武技")
class CANCERINVENTORY_API UQuickBarIns_Skills : public UCancerQuickBarInstance
{
	GENERATED_BODY()

public:
	UQuickBarIns_Skills();
	virtual void RestoreQuickBarIns() override;
	virtual void EquipItemInSlot_Implementation() override;
	virtual void UnequipItemInSlot_Implementation() override;
	UPROPERTY()
	FCancerAbilitySet_GrantedHandles GrantedHandle;
};
