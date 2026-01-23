#pragma once

#include "CoreMinimal.h"
#include "Actions/Action_AddSceneComponent.h"
#include "Action_AddLockingPointComponent.generated.h"

/**
 * 
 */
UCLASS(DisplayName="锁定点组件")
class XYJ_API UAction_AddLockingPointComponent : public UAction_AddSceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere,DisplayName="是否打开锁定")
	bool IsOpen = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,DisplayName="最大锁定距离")
	float MaxLockingDistance = 1000;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,DisplayName="丢失锁定距离")
	float MaxUnLockingDistance = 1000;
	
	
	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;

protected:
	virtual void PostInitComponent_Implementation() override;
};
