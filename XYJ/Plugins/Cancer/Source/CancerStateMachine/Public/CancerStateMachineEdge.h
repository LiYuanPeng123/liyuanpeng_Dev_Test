#pragma once

#include "CoreMinimal.h"
#include "CancerStateMachineEdge.generated.h"

class UCancerStateMachineNode;
class UCancerStateMachineCondition;

/**
 * UCancerStateMachineEdge
 * 通用状态转换边基类
 * 存储从源节点到目标节点的转换逻辑和条件
 */
UCLASS(Blueprintable)
class CANCERSTATEMACHINE_API UCancerStateMachineEdge : public UObject
{
	GENERATED_BODY()

public:
	UCancerStateMachineEdge();

	/** 所属的状态机运行时数据 (由 CreateInstance 设置) */
	struct FCancerStateMachineInstance* OwningInstance;

	/** 转换的目标节点 */
	UPROPERTY()
	TObjectPtr<UCancerStateMachineNode> StartNode;

	/** 转换的目标节点 */
	UPROPERTY()
	TObjectPtr<UCancerStateMachineNode> EndNode;

	/** 转换需要满足的条件列表 (运行时数据) */
	UPROPERTY()
	TArray<TObjectPtr<UCancerStateMachineCondition>> Conditions;

	/** 是否是事件驱动的边 (如果是，则不参与每帧轮询) */
	UPROPERTY()
	bool bIsEventEdge = false;

	/** 优先级 (数值越大优先级越高，在多个转换同时满足时生效) */
	UPROPERTY(EditAnywhere, Category = "Cancer|StateMachine")
	int32 Priority = 0;

	/** 是否连接到节点的“完成引脚” (OnFinished) */
	UPROPERTY()
	bool bIsFinishEdge = false;

	/**
	 * 检查所有条件是否都满足
	 * @param Context 执行上下文
	 * @param StateTime 当前状态已运行的时间
	 * @return 是否可以执行转换
	 */
	UFUNCTION(BlueprintCallable, Category = "Cancer|StateMachine")
	virtual bool CanTransition(UObject* Context, float StateTime = 0.0f) const;

	/** 处理事件触发 (用于 EventEdge 立即跳转) */
	void HandleEventTriggered(UObject* Context);

	/** 检查条件是否满足 (不包括 Tick 时间窗口等需要外部参数的条件) */
	bool CheckConditions(UObject* Context, float StateTime) const;
};
