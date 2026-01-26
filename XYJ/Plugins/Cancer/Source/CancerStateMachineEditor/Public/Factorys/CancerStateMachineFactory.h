#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CancerStateMachineFactory.generated.h"

/**
 * UCancerStateMachineFactory
 * 状态机资产工厂类
 * 用于在编辑器中创建新的 UCancerStateMachineData 资产
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineFactory : public UFactory
{
	GENERATED_BODY()

public:
	UCancerStateMachineFactory();

	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	//~ End UFactory Interface
};
