#include "Factorys/CancerStateMachineTaskFactory.h"
#include "CancerStateMachineTask.h"
#include "Kismet2/KismetEditorUtilities.h"

UCancerStateMachineTaskFactory::UCancerStateMachineTaskFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCancerStateMachineTask::StaticClass();
}

UObject* UCancerStateMachineTaskFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// 创建蓝图类，方便用户编写任务逻辑
	return FKismetEditorUtilities::CreateBlueprint(UCancerStateMachineTask::StaticClass(), InParent, InName, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
}