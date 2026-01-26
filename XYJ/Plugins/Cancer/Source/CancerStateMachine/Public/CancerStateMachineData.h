#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CancerStateMachineData.generated.h"

class UCancerStateMachineNode;
class UCancerStateMachineEdge;

DECLARE_MULTICAST_DELEGATE(FCancerStateMachineFinishedDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FCancerStateMachineRequestSwitchDelegate, UCancerStateMachineNode*);

/**
 * FCancerStateMachineInstance
 * 状态机运行时的实例数据
 */
USTRUCT(BlueprintType)
struct FCancerStateMachineInstance
{
	GENERATED_BODY()

	/** 关联的数据资产 */
	UPROPERTY()
	TObjectPtr<const UCancerStateMachineData> DataAsset;

	/** 拥有者 (用于实例化对象的 Outer) */
	UPROPERTY()
	TObjectPtr<UObject> Owner;

	/** 当状态机运行到结束节点时触发 */
	FCancerStateMachineFinishedDelegate OnStateMachineFinished;

	/** 请求切换状态的委托 (由 Task 监听) */
	FCancerStateMachineRequestSwitchDelegate OnRequestStateSwitch;

	/** 运行时节点映射 (Template -> Instance) */
	UPROPERTY()
	TMap<TObjectPtr<UCancerStateMachineNode>, TObjectPtr<UCancerStateMachineNode>> RuntimeNodesMap;

	/** 运行时的全局边 */
	UPROPERTY()
	TArray<TObjectPtr<UCancerStateMachineEdge>> RuntimeGlobalEdges;

	/** 获取或创建节点实例 (延迟实例化) */
	UCancerStateMachineNode* GetOrCreateNodeInstance(UCancerStateMachineNode* TemplateNode);

	/** 检查转换逻辑，返回第一个满足条件的转换目标节点实例 */
	UCancerStateMachineNode* CheckTransitions(UObject* Context, UCancerStateMachineNode* CurrentState, float StateTime, bool bIsEventTrigger = false);

	/** 处理事件触发导致的直接跳转 */
	void HandleEventTriggered(UObject* Context, UCancerStateMachineEdge* Edge);

	/** 内部使用的排序逻辑 */
	void SortEdges(TArray<TObjectPtr<UCancerStateMachineEdge>>& Edges);
};

/**
 * UCancerStateMachineData
 * 通用状态机数据资产
 * 存储整个状态机的拓扑结构
 */
UCLASS(BlueprintType)
class CANCERSTATEMACHINE_API UCancerStateMachineData : public UDataAsset
{
	GENERATED_BODY()

public:
	UCancerStateMachineData();

	/** 状态机中的所有节点 */
	UPROPERTY(VisibleAnywhere, Category = "Cancer|StateMachine")
	TArray<TObjectPtr<UCancerStateMachineNode>> AllNodes;

	/** 根节点 (没有父节点的节点) */
	UPROPERTY(VisibleAnywhere, Category = "Cancer|StateMachine")
	TArray<TObjectPtr<UCancerStateMachineNode>> RootNodes;

	/** 状态机中的所有边 */
	UPROPERTY(VisibleAnywhere, Category = "Cancer|StateMachine")
	TArray<TObjectPtr<UCancerStateMachineEdge>> AllEdges;

	/** 入口转换 (支持多个，按顺序检查条件决定初始状态) */
	UPROPERTY(VisibleAnywhere, Category = "Cancer|StateMachine")
	TArray<TObjectPtr<UCancerStateMachineEdge>> EntryEdges;

	/** 全局转换 (无论当前处于什么状态，都会检查这些转换。参考 ComboGraph 的 Global Transitions) */
	UPROPERTY(EditAnywhere, Category = "Cancer|StateMachine")
	TArray<TObjectPtr<UCancerStateMachineEdge>> GlobalEdges;

	/** 获取初始启动节点 (模板节点) */
	UCancerStateMachineNode* GetStartNode(UObject* Context) const;

	/** 创建状态机的运行时实例 */
	void CreateInstance(UObject* Outer, FCancerStateMachineInstance& OutInstance) const;

#if WITH_EDITORONLY_DATA
	/** 编辑器使用的图形数据 */
	UPROPERTY()
	TObjectPtr<class UEdGraph> EdGraph;
#endif
};
