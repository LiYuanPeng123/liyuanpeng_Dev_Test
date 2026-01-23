
#pragma once

#include "CoreMinimal.h"
#include "Actions/Action_AddActorComponent.h"
#include "Action_AddIndicatorComponent.generated.h"

/**
 * 
 */
UCLASS(DisplayName="指示器组件")
class XYJ_API UAction_AddIndicatorComponent : public UAction_AddActorComponent
{
	GENERATED_BODY()
public:
	UAction_AddIndicatorComponent();
	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;

	
protected:
	virtual void PostInitComponent_Implementation() override;
	
};
