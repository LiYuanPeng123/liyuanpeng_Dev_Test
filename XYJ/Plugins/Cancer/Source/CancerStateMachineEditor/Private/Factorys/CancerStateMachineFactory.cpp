
#include "Factorys/CancerStateMachineFactory.h"

#include "CancerStateMachineData.h"

UCancerStateMachineFactory::UCancerStateMachineFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCancerStateMachineData::StaticClass();
}

UObject* UCancerStateMachineFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UCancerStateMachineData>(InParent, InClass, InName, Flags);
}
