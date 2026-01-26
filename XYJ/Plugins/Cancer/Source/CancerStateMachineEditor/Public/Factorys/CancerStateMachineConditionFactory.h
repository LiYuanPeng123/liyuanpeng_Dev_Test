#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CancerStateMachineConditionFactory.generated.h"

/**
 * UCancerStateMachineConditionFactory
 * 状态机条件资产工厂类
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineConditionFactory : public UFactory
{
	GENERATED_BODY()

public:
	UCancerStateMachineConditionFactory();

	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	//~ End UFactory Interface
};