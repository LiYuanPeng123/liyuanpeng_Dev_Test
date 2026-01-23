#pragma once

#include "CoreMinimal.h"
#include "Actions/Action_AddActorComponent.h"
#include "Action_AddActorPoolComponent.generated.h"

struct FPoolableActorInfo;
/**
 * 
 */
UCLASS(DisplayName="对象池组件")
class XYJ_API UAction_AddActorPoolComponent : public UAction_AddActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pool)
	TArray<FPoolableActorInfo> PoolableActors;

	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;

protected:
	virtual void PostInitComponent_Implementation() override;
};
