#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "CancerStateMachineData.h"
#include "CancerStateMachineNode.h"
#include "CancerStateMachineEdge.h"
#include "GameplayTask_StartStateMachine.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCancerStateMachineSignature, UCancerStateMachineNode*, StateNode);

/**
 * UGameplayTask_StartStateMachine
 * 状态机启动任务 (GameplayTask 模式)
 * 参考 ComboGraph 设计，实现无组件、异步驱动的状态机
 */
UCLASS()
class CANCERSTATEMACHINE_API UGameplayTask_StartStateMachine : public UGameplayTask
{
	GENERATED_BODY()

public:
	UGameplayTask_StartStateMachine(const FObjectInitializer& ObjectInitializer);

	/** 
	 * 启动状态机任务
	 * @param TaskOwner 拥有该任务的对象 (需实现 IGameplayTaskOwnerInterface 或为 Actor)
	 * @param InData 状态机配置资产
	 * @param ContextOverride 可选：覆盖执行上下文 (默认使用 TaskOwner)
	 * @param bInDebug 是否开启调试模式
	 */
	UFUNCTION(BlueprintCallable, Category = "Cancer|StateMachine", meta = (DefaultToSelf = "TaskOwner", BlueprintInternalUseOnly = "TRUE"))
	static UGameplayTask_StartStateMachine* StartCancerStateMachine(UObject* TaskOwner, UCancerStateMachineData* InData, UObject* ContextOverride = nullptr, bool bInDebug = false);

	/** 当前进入新状态时的回调 */
	UPROPERTY(BlueprintAssignable)
	FCancerStateMachineSignature OnStateEntered;

	/** 退出旧状态时的回调 */
	UPROPERTY(BlueprintAssignable)
	FCancerStateMachineSignature OnStateExited;

	/** 状态机结束时的回调 */
	UPROPERTY(BlueprintAssignable)
	FCancerStateMachineSignature OnStateMachineFinished;

	/** 手动停止状态机 */
	UFUNCTION(BlueprintCallable, Category = "Cancer|StateMachine")
	void StopStateMachine();

protected:
	/** 任务启动 */
	virtual void Activate() override;
	
	/** 每帧更新 */
	virtual void TickTask(float DeltaTime) override;

	/** 任务清理 */
	virtual void OnDestroy(bool bInOwnerFinished) override;

	/** 切换状态逻辑 */
	void SwitchState(UCancerStateMachineNode* NewState);

	/** 绑定/解绑当前状态下所有边的条件触发器 (针对 Event 类型的条件) */
	void BindCurrentStateTriggers();
	void UnbindCurrentStateTriggers();

	/** 检查状态转换 */
	void CheckTransitions(bool bIsEventTrigger = false);

protected:
	/** 状态机数据资产 */
	UPROPERTY()
	TObjectPtr<UCancerStateMachineData> StateMachineData;

	/** 状态机运行时实例 */
	UPROPERTY()
	FCancerStateMachineInstance StateMachineInstance;

	/** 可选的上下文覆盖 */
	UPROPERTY()
	TObjectPtr<UObject> ContextOverride;

private:
	/** 当前运行的状态 */
	UPROPERTY()
	TObjectPtr<UCancerStateMachineNode> CurrentState;

	/** 当前状态已经运行的时间 */
	float StateTime;

	bool bDebugMode;

	/** 获取任务关联的 Actor 上下文 */
	UObject* GetContext() const;
};
