#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "CancerStateMachineTask.generated.h"

/**
 * UCancerStateMachineTask
 * 状态机任务基类
 * 继承自 UGameplayTask，支持独立 Tick 和资源管理
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class CANCERSTATEMACHINE_API UCancerStateMachineTask : public UGameplayTask
{
	GENERATED_BODY()

public:
	UCancerStateMachineTask(const FObjectInitializer& ObjectInitializer);

	/** 
	 * 辅助函数：将任务注册到指定的 Owner (绕过 InitTask 的 protected 限制)
	 */
	void RegisterTask(IGameplayTaskOwnerInterface& InTaskOwner, uint8 InPriority);

	/** 
	 * 任务初始化 (由 Node 调用)
	 */
	virtual void InitializeTask(UObject* InContext);

	/** 获取执行上下文 */
	UObject* GetContext() const { return ContextPtr.Get(); }

	/** 结束任务并通知所属节点完成 (会触发 OnFinished 引脚跳转) */
	UFUNCTION(BlueprintCallable, Category = "Cancer|StateMachine")
	void FinishNode();

protected:
	//~ Begin UGameplayTask Interface
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~ End UGameplayTask Interface

	/** 蓝图可重写的事件 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Cancer|StateMachine", meta = (DisplayName = "OnActivated"))
	void BP_OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Cancer|StateMachine", meta = (DisplayName = "OnTick"))
	void BP_OnTick(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cancer|StateMachine", meta = (DisplayName = "OnDeactivated"))
	void BP_OnDeactivated();

private:
	UPROPERTY()
	TWeakObjectPtr<UObject> ContextPtr;
};