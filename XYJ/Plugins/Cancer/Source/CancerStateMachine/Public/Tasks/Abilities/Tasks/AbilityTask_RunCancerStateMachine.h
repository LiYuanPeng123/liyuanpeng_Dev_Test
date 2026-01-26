#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "CancerStateMachineData.h"
#include "CancerStateMachineNode.h"
#include "AbilityTask_RunCancerStateMachine.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCancerStateMachineAbilityTaskDelegate, UCancerStateMachineNode*, StateNode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCancerStateMachineEventReceivedDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

/**
 * UAbilityTask_RunCancerStateMachine
 * 在 Gameplay Ability 中运行 Cancer 状态机的任务
 * 参考 ComboGraphAbilityTask_StartGraph 实现
 */
UCLASS()
class CANCERSTATEMACHINE_API UAbilityTask_RunCancerStateMachine : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_RunCancerStateMachine(const FObjectInitializer& ObjectInitializer);

	/**
	 * 启动状态机任务 (Ability Task 版本)
	 * @param OwningAbility 拥有该任务的 Ability
	 * @param InData 状态机配置资产
	 * @param ContextOverride 可选：覆盖执行上下文 (默认使用 AvatarActor)
	 * @param bInDebug 是否开启调试模式
	 */
	UFUNCTION(BlueprintCallable, Category = "Cancer|StateMachine|Abilities", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_RunCancerStateMachine* RunCancerStateMachine(UGameplayAbility* OwningAbility, UCancerStateMachineData* InData, UObject* ContextOverride = nullptr, bool bInDebug = false);

	/** 当状态机进入新状态时的回调 */
	UPROPERTY(BlueprintAssignable)
	FCancerStateMachineAbilityTaskDelegate OnStateEntered;

	/** 当状态机退出状态时的回调 */
	UPROPERTY(BlueprintAssignable)
	FCancerStateMachineAbilityTaskDelegate OnStateExited;

	/** 当状态机结束时的回调 */
	UPROPERTY(BlueprintAssignable)
	FCancerStateMachineAbilityTaskDelegate OnStateMachineFinished;

	/** 当接收到 Gameplay Event 时的回调 (参考 ComboGraph) */
	UPROPERTY(BlueprintAssignable)
	FCancerStateMachineEventReceivedDelegate EventReceived;

	/** 手动停止状态机 */
	UFUNCTION(BlueprintCallable, Category = "Cancer|StateMachine|Abilities")
	void StopStateMachine();

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

	//~ Begin UGameplayTask Interface
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~ End UGameplayTask Interface

	/** 切换状态逻辑 */
	void SwitchState(UCancerStateMachineNode* NewState);

	/** 绑定/解绑当前状态下所有边的条件触发器 (针对 Event 类型的条件) */
	void BindCurrentStateTriggers();
	void UnbindCurrentStateTriggers();

	/** 响应子任务收到的事件 */
	UFUNCTION()
	void HandleEventReceived(FGameplayEventData Payload);

protected:

	/** 检查转换逻辑 */
	void CheckTransitions(bool bIsEventTrigger = false);

private:
	/** 内部使用的事件监听子任务 */
	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitGameplayEvent> WaitEventTask;

	/** 当前正在运行的状态 */
	UPROPERTY()
	TObjectPtr<UCancerStateMachineNode> CurrentState;

	/** 当前状态已经运行的时间 */
	UPROPERTY()
	float StateTime;

	bool bDebugMode;

	/** 获取任务关联的上下文 (Actor) */
	UObject* GetContext() const;
};
