#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "CancerStateMachineCondition.generated.h"

class UCancerStateMachineNode;
class UCancerStateMachineEdge;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCancerConditionTriggered, UObject*, Context);

/**
 * UCancerStateMachineCondition
 * 通用状态转换条件基类
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class CANCERSTATEMACHINE_API UCancerStateMachineCondition : public UObject
{
	GENERATED_BODY()

public:
	/** 获取条件的显示名称 */
	virtual FText GetConditionDisplayName() const { return FText::FromString(GetClass()->GetName()); }

	/** 所属的边 */
	UPROPERTY()
	TObjectPtr<class UCancerStateMachineEdge> OwningEdge;
};

/**
 * UCancerStateMachineCondition_Tick
 * 轮询式条件基类 (Tick 检测)
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class CANCERSTATEMACHINE_API UCancerStateMachineCondition_Tick : public UCancerStateMachineCondition
{
	GENERATED_BODY()

public:
	/**
	 * 判断条件是否满足
	 * @param Context 执行上下文
	 * @param SourceNode 源节点
	 * @param StateTime 状态运行时间
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Cancer|StateMachine")
	bool IsSatisfied(UObject* Context, const class UCancerStateMachineNode* SourceNode, float StateTime) const;

	virtual bool IsSatisfied_Implementation(UObject* Context, const class UCancerStateMachineNode* SourceNode, float StateTime) const { return true; }
};

/**
 * UCancerStateMachineCondition_Event
 * 事件驱动式条件基类 (如按键触发)
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class CANCERSTATEMACHINE_API UCancerStateMachineCondition_Event : public UCancerStateMachineCondition
{
	GENERATED_BODY()

public:
	/** 当条件达成时触发的回调 */
	FOnCancerConditionTriggered OnConditionTriggered;

	/** 进入状态时激活条件绑定 */
	UFUNCTION(BlueprintNativeEvent, Category = "Cancer|StateMachine")
	void ActivateCondition(UObject* Context);
	virtual void ActivateCondition_Implementation(UObject* Context) {}

	/** 退出状态时清理条件绑定 */
	UFUNCTION(BlueprintNativeEvent, Category = "Cancer|StateMachine")
	void DeactivateCondition(UObject* Context);
	virtual void DeactivateCondition_Implementation(UObject* Context) {}

protected:
	/** 辅助函数：通知条件已触发 */
	UFUNCTION(BlueprintCallable, Category = "Cancer|StateMachine")
	void BroadcastTriggered(UObject* Context);
};

/**
 * UCancerStateMachineCondition_TimeWindow
 * 时间窗口条件 (属于 Tick 类型)
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Time Window"))
class CANCERSTATEMACHINE_API UCancerStateMachineCondition_TimeWindow : public UCancerStateMachineCondition_Tick
{
	GENERATED_BODY()

public:
	UCancerStateMachineCondition_TimeWindow();

	/** 转换触发的起始时间 */
	UPROPERTY(EditAnywhere, Category = "Time")
	float StartTime;

	/** 转换触发的截止时间 (如果为 0，则不限制) */
	UPROPERTY(EditAnywhere, Category = "Time")
	float EndTime;

	virtual bool IsSatisfied_Implementation(UObject* Context, const UCancerStateMachineNode* SourceNode,
	                                        float StateTime) const override;
};

/**
 * UCancerStateMachineCondition_TransitionInput
 * 按键输入条件 (属于 Event 类型)
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Transition Input"))
class CANCERSTATEMACHINE_API UCancerStateMachineCondition_TransitionInput : public UCancerStateMachineCondition_Event
{
	GENERATED_BODY()

public:
	/** 关联的输入动作 */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> InputAction;

	/** 输入检测类型 */
	UPROPERTY(EditAnywhere, Category = "Input")
	ETriggerEvent TriggerEvent = ETriggerEvent::Started;

	virtual void ActivateCondition_Implementation(UObject* Context) override;
	virtual void DeactivateCondition_Implementation(UObject* Context) override;

protected:
	void HandleInputAction();

	/** 绑定的句柄，用于解绑 */
	uint32 BindingHandle;
};
