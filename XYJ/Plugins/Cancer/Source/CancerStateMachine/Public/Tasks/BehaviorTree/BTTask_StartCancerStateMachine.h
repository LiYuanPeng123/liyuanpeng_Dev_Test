#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CancerStateMachineData.h"
#include "BTTask_StartCancerStateMachine.generated.h"

class UGameplayTask_StartStateMachine;
/**
 * UBTTask_RunCancerStateMachine
 * 在行为树中运行 Cancer 状态机的任务
 * 参考 ComboGraphBTTask_RunComboGraph 实现
 */
UCLASS(DisplayName = "Run Cancer State Machine")
class CANCERSTATEMACHINE_API UBTTask_StartCancerStateMachine : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_StartCancerStateMachine(const FObjectInitializer& ObjectInitializer);

	/** 状态机资产 */
	UPROPERTY(EditAnywhere, Category = "Cancer|StateMachine")
	TObjectPtr<UCancerStateMachineData> StateMachineData;

	/** 是否开启调试模式 */
	UPROPERTY(EditAnywhere, Category = "Cancer|StateMachine")
	bool bDebugMode;

	//~ Begin UBTTaskNode Interface
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual FString GetStaticDescription() const override;
	//~ End UBTTaskNode Interface

protected:
	/** 当状态机结束时的回调 */
	UFUNCTION()
	void HandleStateMachineFinished(UCancerStateMachineNode* FinalState);

private:
	/** 清理任务 */
	void CleanUp();

	UPROPERTY()
	TObjectPtr<UGameplayTask_StartStateMachine> ActiveExecutor;

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
};
