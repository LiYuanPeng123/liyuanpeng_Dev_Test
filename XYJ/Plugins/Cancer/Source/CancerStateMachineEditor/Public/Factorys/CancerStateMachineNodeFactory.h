// Copyright 2026 Cancer State Machine. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CancerStateMachineNodeFactory.generated.h"

/**
 * UCancerStateMachineNodeFactory
 * 状态机节点蓝图资产工厂。
 * 允许用户在内容浏览器中创建 UCancerStateMachineNodeBlueprint 类型的资产。
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineNodeFactory : public UFactory
{
	GENERATED_BODY()

public:
	UCancerStateMachineNodeFactory();

	//~ Begin UFactory Interface
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	//~ End UFactory Interface

	/** 创建蓝图时的父类 */
	UPROPERTY(EditAnywhere, Category = "Cancer|StateMachine")
	TSubclassOf<class UCancerStateMachineNode> ParentClass;
};
