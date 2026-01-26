#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "CancerStateMachineNode.generated.h"

struct FCancerStateMachineInstance;
class UCancerStateMachineEdge;

/**
 * UCancerStateMachineNode
 * 通用状态节点基类
 * 代表状态机中的一个具体状态
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class CANCERSTATEMACHINE_API UCancerStateMachineNode : public UObject
{
	GENERATED_BODY()

public:
	UCancerStateMachineNode();

	/** 该状态节点的名称 (在编辑器中自动同步) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Config")
	FText NodeName;

	/** 该状态关联的 GameplayTag (参考 ComboGraph，进入状态自动添加，退出移除) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Config")
	FGameplayTag StateTag;

	/** 子节点 (从此节点出发连接到的节点) */
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UCancerStateMachineNode>> ChildrenNodes;

	/** 父节点 (连接到此节点的节点) */
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UCancerStateMachineNode>> ParentNodes;

	/** 出边集合 (所有从本节点出发的转换) */
	UPROPERTY()
	TArray<TObjectPtr<UCancerStateMachineEdge>> OutgoingEdges;

	/** 执行此节点的任务所有者 (用于启动子任务) */
	UPROPERTY()
	TScriptInterface<class IGameplayTaskOwnerInterface> TaskOwner;

	/** 驱动此节点的状态机任务 */
	UPROPERTY()
	TObjectPtr<class UGameplayTask> OwningTask;

	/** 所属的状态机实例数据 (如果是子状态机，则指向子状态机实例) */
	FCancerStateMachineInstance* ParentInstance;

	/** 是否已经初始化过 */
	UPROPERTY()
	bool bInitialized;

	/** 当节点主动标记为“完成”时触发的通知 (仅由状态机内部监听) */
	FSimpleMulticastDelegate OnNodeCompleted;

	/** 节点是否已标记为完成 */
	bool bIsNodeFinished = false;

	/** 处理状态标签 (添加/移除) */
	void HandleStateTag(UObject* Context, bool bAdd);

	/** 
	 * 内部初始化函数 (由状态机任务在第一次进入时调用)
	 */
	void InitializeNode(UObject* Context, class UGameplayTask* InOwningTask, FCancerStateMachineInstance* InParentInstance = nullptr);

	/**
	 * 标记当前节点已完成 (会触发 OnFinished 引脚的跳转)
	 */
	UFUNCTION(BlueprintCallable, Category = "Cancer|StateMachine")
	void FinishNode();

	/**
	 * 结束整个状态机 (会递归查找最顶层的 OwningTask 并结束它)
	 */
	UFUNCTION(BlueprintCallable, Category = "Cancer|StateMachine")
	void FinishStateMachine();

	/**
	 * 节点初始化 (资产加载或状态机启动时调用一次)
	 * @param Context 执行上下文
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Cancer|StateMachine")
	void OnInitialized(UObject* Context);
	virtual void OnInitialized_Implementation(UObject* Context) {}

	/**
	 * 节点激活 (进入状态)
	 * @param Context 执行上下文
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Cancer|StateMachine")
	void OnActivated(UObject* Context);
	virtual void OnActivated_Implementation(UObject* Context);

	/**
	 * 节点停用 (退出状态)
	 * @param Context 执行上下文
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Cancer|StateMachine")
	void OnDeactivated(UObject* Context);
	virtual void OnDeactivated_Implementation(UObject* Context);

	/**
	 * 节点每帧更新
	 * @param Context 执行上下文
	 * @param DeltaTime 帧间隔时间
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Cancer|StateMachine")
	void OnUpdate(UObject* Context, float DeltaTime);
	virtual void OnUpdate_Implementation(UObject* Context, float DeltaTime) {}

	/**
	 * 响应 Gameplay Event 事件
	 * @param EventTag 事件标签
	 * @param EventData 事件数据
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Cancer|StateMachine")
	void OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
	virtual void OnEventReceived_Implementation(FGameplayTag EventTag, FGameplayEventData EventData) {}
};

/**
 * UCancerStateMachineNode_Task
 * 任务型状态节点 (Type A)
 */
UCLASS()
class CANCERSTATEMACHINE_API UCancerStateMachineNode_Task : public UCancerStateMachineNode
{
	GENERATED_BODY()

public:
	UCancerStateMachineNode_Task();

	/** 在此状态下要执行的任务列表 */
	UPROPERTY(EditAnywhere, Instanced, Category = "State Config")
	TArray<TObjectPtr<class UCancerStateMachineTask>> StateTasks;

	virtual void OnActivated_Implementation(UObject* Context) override;
	virtual void OnDeactivated_Implementation(UObject* Context) override;

protected:
	/** 正在运行任务实例 */
	UPROPERTY()
	TArray<TObjectPtr<class UCancerStateMachineTask>> ActiveTaskInstances;
};
