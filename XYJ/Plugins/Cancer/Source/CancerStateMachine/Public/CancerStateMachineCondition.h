#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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

	virtual bool IsSatisfied_Implementation(UObject* Context, const class UCancerStateMachineNode* SourceNode,
	                                        float StateTime) const { return true; }
};

/**
 * UCancerStateMachineCondition_Event
 * 事件驱动式条件
 */
UCLASS(Blueprintable, EditInlineNew)
class CANCERSTATEMACHINE_API UCancerStateMachineCondition_Event : public UCancerStateMachineCondition
{
	GENERATED_BODY()

public:
	/** 触发跳转的事件标签 */
	UPROPERTY(EditAnywhere, Category = "Event")
	FGameplayTag TriggerTag;

	/** 是否要求精确匹配 (False 则允许子标签触发) */
	UPROPERTY(EditAnywhere, Category = "Event")
	bool bExactMatch = true;
};


/**
 * UCancerStateMachineCondition_TimeWindow
 * 延迟条件(属于 Tick 类型)
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Delay Time"))
class CANCERSTATEMACHINE_API UCancerStateMachineCondition_DelayTime : public UCancerStateMachineCondition_Tick
{
	GENERATED_BODY()

public:
	UCancerStateMachineCondition_DelayTime();

	/** 转换触发的时间 */
	UPROPERTY(EditAnywhere, Category = "Time")
	float DelayTime;

	virtual bool IsSatisfied_Implementation(UObject* Context, const UCancerStateMachineNode* SourceNode,
	                                        float StateTime) const override;
};
