#pragma once

#include "CoreMinimal.h"
#include "GameplayTasksComponent.h"
#include "CancerStateMachineData.h"
#include "CancerStateMachineComponent.generated.h"

class UGameplayTask_StartStateMachine;
/**
 * UCancerStateMachineComponent
 */
UCLASS(ClassGroup=(Cancer), meta=(BlueprintSpawnableComponent))
class CANCERSTATEMACHINE_API UCancerStateMachineComponent : public UGameplayTasksComponent
{
	GENERATED_BODY()

public:
	UCancerStateMachineComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	
};
