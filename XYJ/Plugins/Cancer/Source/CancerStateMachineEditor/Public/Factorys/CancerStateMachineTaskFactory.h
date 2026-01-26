#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CancerStateMachineTaskFactory.generated.h"

/**
 * UCancerStateMachineTaskFactory
 * 状态机任务资产工厂类
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineTaskFactory : public UFactory
{
	GENERATED_BODY()

public:
	UCancerStateMachineTaskFactory();

	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	//~ End UFactory Interface
};