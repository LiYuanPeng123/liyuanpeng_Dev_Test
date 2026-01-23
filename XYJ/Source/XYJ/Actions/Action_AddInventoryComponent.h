#pragma once
#include "CoreMinimal.h"
#include "Actions/Action_AddActorComponent.h"
#include "Action_AddInventoryComponent.generated.h"

class UCancerInventoryComponent;


UCLASS(DisplayName="背包组件")
class XYJ_API UAction_AddInventoryComponent : public UAction_AddActorComponent
{
	GENERATED_BODY()

protected:
	virtual void PreInitComponent_Implementation() override;

	virtual void FinalizeAfterComponent_Implementation() override;
};
