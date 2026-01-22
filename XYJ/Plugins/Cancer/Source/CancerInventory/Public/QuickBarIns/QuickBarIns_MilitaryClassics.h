#pragma once

#include "CoreMinimal.h"
#include "CancerQuickBarInstance.h"
#include "QuickBarIns_MilitaryClassics.generated.h"

/** 
 *  功法：明确插槽(2) 主副功法 管理武技
 */
UCLASS(DisplayName="功法")
class CANCERINVENTORY_API UQuickBarIns_MilitaryClassics : public UCancerQuickBarInstance
{
	GENERATED_BODY()
public:
	UQuickBarIns_MilitaryClassics();
	virtual void EquipItemInSlot_Implementation() override;
	virtual void UnequipItemInSlot_Implementation() override;
};
