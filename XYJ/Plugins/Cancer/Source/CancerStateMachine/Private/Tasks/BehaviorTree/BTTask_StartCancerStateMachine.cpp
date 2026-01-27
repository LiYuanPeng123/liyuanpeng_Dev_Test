
#include "Tasks/BehaviorTree/BTTask_StartCancerStateMachine.h"

#include "AIController.h"
#include "Tasks/GameplayTask_StartStateMachine.h"
#include "VisualLogger/VisualLogger.h"

UBTTask_StartCancerStateMachine::UBTTask_StartCancerStateMachine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Run Cancer State Machine";
	
	// 允许为每个节点实例创建对象，以便存储状态
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
	bDebugMode = false;
}

EBTNodeResult::Type UBTTask_StartCancerStateMachine::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!MyController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* MyPawn = MyController->GetPawn();
	if (!MyPawn || !StateMachineData)
	{
		return EBTNodeResult::Failed;
	}

	CachedOwnerComp = &OwnerComp;

	// 启动状态机执行器
	ActiveExecutor = UGameplayTask_StartStateMachine::StartCancerStateMachine(MyPawn, StateMachineData, nullptr, bDebugMode);
	if (!ActiveExecutor)
	{
		UE_VLOG(MyPawn, LogTemp, Error, TEXT("BTTask_RunCancerStateMachine: Failed to create Executor"));
		return EBTNodeResult::Failed;
	}

	// 绑定结束回调
	ActiveExecutor->OnStateMachineFinished.AddDynamic(this, &UBTTask_StartCancerStateMachine::HandleStateMachineFinished);
	
	// 准备启动
	ActiveExecutor->ReadyForActivation();

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_StartCancerStateMachine::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CleanUp();
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_StartCancerStateMachine::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	CleanUp();
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

FString UBTTask_StartCancerStateMachine::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), StateMachineData ? *StateMachineData->GetName() : TEXT("None"));
}

void UBTTask_StartCancerStateMachine::HandleStateMachineFinished(UCancerStateMachineNode* FinalState)
{
	if (CachedOwnerComp.IsValid())
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_StartCancerStateMachine::CleanUp()
{
	if (ActiveExecutor)
	{
		ActiveExecutor->OnStateMachineFinished.RemoveAll(this);
		ActiveExecutor->EndTask();
		ActiveExecutor = nullptr;
	}
}
