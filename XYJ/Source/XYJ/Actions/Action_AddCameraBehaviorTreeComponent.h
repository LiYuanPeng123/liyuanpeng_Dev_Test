#pragma once

#include "CoreMinimal.h"
#include "Actions/Action_AddSceneComponent.h"
#include "Action_AddCameraBehaviorTreeComponent.generated.h"

class UBehaviorTree;
/**
 * 
 */
UCLASS(DisplayName="摄像机组件")
class XYJ_API UAction_AddCameraBehaviorTreeComponent : public UAction_AddSceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta=(ExposeOnSpawn))
	TSoftObjectPtr<UBehaviorTree> CameraBehaviorTree;

	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;

protected:
	virtual void PostInitComponent_Implementation() override;
	virtual void FinalizeAfterComponent_Implementation() override;
};
