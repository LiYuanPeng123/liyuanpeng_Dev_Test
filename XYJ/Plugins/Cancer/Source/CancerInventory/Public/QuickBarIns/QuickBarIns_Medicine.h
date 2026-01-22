#pragma once

#include "CoreMinimal.h"
#include "CancerQuickBarInstance.h"
#include "QuickBarIns_Medicine.generated.h"

/**
 * 
 */
UCLASS(DisplayName="丹药")
class CANCERINVENTORY_API UQuickBarIns_Medicine : public UCancerQuickBarInstance
{
	GENERATED_BODY()

public:
	UQuickBarIns_Medicine();
	virtual void EquipItemInSlot_Implementation() override;

	virtual bool ApplyItemInSlot_Implementation() override;
};
