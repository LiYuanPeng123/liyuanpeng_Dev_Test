
#include "Factorys/CancerStateMachineConditionFactory.h"

#include "CancerStateMachineCondition.h"
#include "Kismet2/KismetEditorUtilities.h"

UCancerStateMachineConditionFactory::UCancerStateMachineConditionFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCancerStateMachineCondition::StaticClass();
}

UObject* UCancerStateMachineConditionFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// 默认创建一个蓝图类，这样用户可以重写 IsSatisfied 逻辑
	return FKismetEditorUtilities::CreateBlueprint(UCancerStateMachineCondition::StaticClass(), InParent, InName, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
}