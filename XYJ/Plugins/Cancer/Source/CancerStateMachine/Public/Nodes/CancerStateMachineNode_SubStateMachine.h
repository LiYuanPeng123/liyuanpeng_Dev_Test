#pragma once

#include "CoreMinimal.h"
#include "CancerStateMachineNode.h"
#include "CancerStateMachineData.h"
#include "CancerStateMachineNode_SubStateMachine.generated.h"

/**
 * UCancerStateMachineNode_SubStateMachine
 * 子状态机节点 (实现层级状态机 HSM)
 * 当进入此状态时，会启动一个内部的子状态机
 */
UCLASS(Blueprintable, BlueprintType)
class CANCERSTATEMACHINE_API UCancerStateMachineNode_SubStateMachine : public UCancerStateMachineNode
{
	GENERATED_BODY()

public:
	UCancerStateMachineNode_SubStateMachine();

	/** 子状态机数据资产 */
	UPROPERTY(EditAnywhere, Category = "SubStateMachine")
	TObjectPtr<UCancerStateMachineData> SubStateMachineData;

	//~ Begin UCancerStateMachineNode Interface
	virtual void OnActivated_Implementation(UObject* Context) override;
	virtual void OnDeactivated_Implementation(UObject* Context) override;
	virtual void OnUpdate_Implementation(UObject* Context, float DeltaTime) override;
	//~ End UCancerStateMachineNode Interface

protected:
	/** 子状态机运行时实例 */
	UPROPERTY()
	FCancerStateMachineInstance SubStateMachineInstance;

	/** 子状态机当前状态 */
	UPROPERTY()
	TObjectPtr<UCancerStateMachineNode> CurrentSubState;

	/** 子状态机运行时间 */
	float SubStateTime;

	/** 切换子状态逻辑 */
	void SwitchSubState(UObject* Context, UCancerStateMachineNode* NewState);

	/** 请求切换状态的回调 (不带 Context 参数，由 OnRequestStateSwitch 调用) */
	void HandleRequestSubStateSwitch(UCancerStateMachineNode* NewState);

	/** 获取当前的执行上下文 */
	UObject* GetContext() const;

	/** 检查子状态机转换 */
	void CheckSubTransitions(UObject* Context);
	
	/** 绑定/解绑子状态机当前状态的触发器 */
	void BindSubStateTriggers(UObject* Context);
	void UnbindSubStateTriggers(UObject* Context);

	/** 响应子状态机运行结束 */
	void HandleSubStateMachineFinished();

	bool bIsWaitingForFinishTransition;
};
