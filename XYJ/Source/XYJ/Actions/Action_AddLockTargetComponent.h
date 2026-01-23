#pragma once

#include "CoreMinimal.h"
#include "Actions/Action_AddActorComponent.h"
#include "Action_AddLockTargetComponent.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS(DisplayName="锁定组件")
class XYJ_API UAction_AddLockTargetComponent : public UAction_AddActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="最大锁定距离")
	float MaxLockingDistance = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="锁定点过滤标签")
	FGameplayTagContainer LockContainer;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, DisplayName="锁定点指示器UI")
	TSoftClassPtr<UWidgetComponent> LockingWidgetComponentClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, DisplayName="锁定点UI")
	TSoftClassPtr<UUserWidget> LockingPointWidgetClass;

	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;

protected:
	virtual void PostInitComponent_Implementation() override;
};
