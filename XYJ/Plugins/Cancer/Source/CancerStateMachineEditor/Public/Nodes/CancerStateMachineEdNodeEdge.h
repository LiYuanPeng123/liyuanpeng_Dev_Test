#pragma once

#include "CoreMinimal.h"
#include "CancerStateMachineEdNodeBase.h"
#include "CancerStateMachineEdge.h"
#include "CancerStateMachineEdNodeEdge.generated.h"

class UCancerStateMachineEdNode;

/**
 * UCancerStateMachineEdNodeEdgeBase
 * 编辑器中转换节点的基类
 */
UCLASS(Abstract)
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNodeEdgeBase : public UCancerStateMachineEdNodeBase
{
	GENERATED_BODY()

public:
	UCancerStateMachineEdNodeEdgeBase();

	/** 关联的运行时边数据 */
	UPROPERTY()
	TObjectPtr<UCancerStateMachineEdge> RuntimeEdge;

	/** 优先级 (数值越大优先级越高) */
	UPROPERTY(EditAnywhere, Category = "Transition Config")
	int32 Priority = 0;

	/** 是否是“节点完成”时的自动转换边 (对应 OnFinished 引脚) */
	UPROPERTY()
	bool bIsFinishEdge = false;

	/** 获取该边连接的起始节点 */
	UCancerStateMachineEdNode* GetStartNode() const;

	/** 获取该边连接的结束节点 */
	UCancerStateMachineEdNode* GetEndNode() const;

	/** 将编辑器节点的条件同步到运行时对象 */
	virtual void SyncRuntimeConditions() PURE_VIRTUAL(UCancerStateMachineEdNodeEdgeBase::SyncRuntimeConditions, );
	
	/** 从运行时对象同步条件到编辑器节点 */
	virtual void SyncEditorConditions() PURE_VIRTUAL(UCancerStateMachineEdNodeEdgeBase::SyncEditorConditions, );

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UEdGraphNode Interface

	//~ Begin UCancerStateMachineEdNodeBase Interface
	virtual UEdGraphPin* GetOutputPin() const override;
	//~ End UCancerStateMachineEdNodeBase Interface
};

/**
 * UCancerStateMachineEdNodeEdge
 * 轮询式转换节点 (Tick)
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNodeEdge : public UCancerStateMachineEdNodeEdgeBase
{
	GENERATED_BODY()

public:
	UCancerStateMachineEdNodeEdge();

	/** 转换条件列表 (仅限 Tick 类型) */
	UPROPERTY(EditAnywhere, Instanced, Category = "Transition Config", meta = (AllowedClasses = "/Script/CancerStateMachine.CancerStateMachineCondition_Tick"))
	TArray<TObjectPtr<class UCancerStateMachineCondition>> Conditions;

	//~ Begin UCancerStateMachineEdNodeEdgeBase Interface
	virtual void SyncRuntimeConditions() override;
	virtual void SyncEditorConditions() override;
	//~ End UCancerStateMachineEdNodeEdgeBase Interface

	//~ Begin UEdGraphNode Interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode Interface
};

/**
 * UCancerStateMachineEdNodeEventEdge
 * 事件驱动式转换节点 (Event)
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNodeEventEdge : public UCancerStateMachineEdNodeEdgeBase
{
	GENERATED_BODY()

public:
	UCancerStateMachineEdNodeEventEdge();

	/** 转换条件列表 (仅限 Event 类型) */
	UPROPERTY(EditAnywhere, Instanced, Category = "Transition Config", meta = (AllowedClasses = "/Script/CancerStateMachine.CancerStateMachineCondition_Event"))
	TArray<TObjectPtr<class UCancerStateMachineCondition>> Conditions;

	//~ Begin UCancerStateMachineEdNodeEdgeBase Interface
	virtual void SyncRuntimeConditions() override;
	virtual void SyncEditorConditions() override;
	//~ End UCancerStateMachineEdNodeEdgeBase Interface

	//~ Begin UEdGraphNode Interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode Interface
};
