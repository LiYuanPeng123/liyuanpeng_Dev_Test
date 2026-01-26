// Copyright 2026 Cancer State Machine. All Rights Reserved.

#include "Factorys/CancerStateMachineNodeFactory.h"
#include "CancerStateMachineNode.h"
#include "CancerStateMachineNodeBlueprint.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/SClassPickerDialog.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

UCancerStateMachineNodeFactory::UCancerStateMachineNodeFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCancerStateMachineNodeBlueprint::StaticClass();
	ParentClass = UCancerStateMachineNode::StaticClass();
}

bool UCancerStateMachineNodeFactory::ConfigureProperties()
{
	// 如果需要用户选择特定的子类作为父类，可以在这里弹出选择对话框
	// 暂时默认使用 UCancerStateMachineNode
	return true;
}

UObject* UCancerStateMachineNodeFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(InClass->IsChildOf(UCancerStateMachineNodeBlueprint::StaticClass()));

	if (!FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		return nullptr;
	}

	// 创建标准蓝图，但设置父类为 ParentClass
	UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(
		ParentClass, 
		InParent, 
		InName, 
		BPTYPE_Normal, 
		UBlueprint::StaticClass(), 
		UBlueprintGeneratedClass::StaticClass()
	);

	if (NewBP)
	{
		// 创建默认的事件图表，确保蓝图不是“仅细节”模式
		if (NewBP->UbergraphPages.Num() == 0)
		{
			UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(NewBP, "EventGraph", UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
			NewBP->UbergraphPages.Add(NewGraph);
		}

		// 强制编译蓝图
		FKismetEditorUtilities::CompileBlueprint(NewBP);
	}

	return NewBP;
}
