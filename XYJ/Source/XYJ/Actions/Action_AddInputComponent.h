
#pragma once

#include "CoreMinimal.h"
#include "Actions/Action_AddActorComponent.h"
#include "Data/CancerInputConfig.h"
#include "Action_AddInputComponent.generated.h"

/**
 * 
 */
UCLASS(DisplayName="输入组件")
class XYJ_API UAction_AddInputComponent : public UAction_AddActorComponent
{
	GENERATED_BODY()
	public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Setup")
	TSoftObjectPtr<UCancerInputConfig> InputConfig;
	
	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;

protected:
	virtual void PreInitComponent_Implementation() override;
	virtual void FinalizeAfterComponent_Implementation() override;
};
