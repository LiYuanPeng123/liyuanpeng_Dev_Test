#include "Behavitor/BTTask_RunEQSAndMove.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_RunEQSAndMove::UBTTask_RunEQSAndMove()
{
	// 初始化默认参数，开启Tick并使用节点实例
	bNotifyTick = true;
	bCreateNodeInstance = true;
	RunMode = EEnvQueryRunMode::SingleResult;
	AcceptableRadius = 50.f;
	bUsePathfinding = true;
	bProjectGoalOnNavigation = true;
}

EBTNodeResult::Type UBTTask_RunEQSAndMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 重置状态并启动首个EQS查询
	bQueryFinished = false;
	bMoveRequested = false;
	AI = OwnerComp.GetAIOwner();
	InstanceOwnerComp = &OwnerComp;

	// 失败保护：缺少查询模板或AI无效
	if (!QueryTemplate || !AI.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	UWorld* World = AI->GetWorld();
	if (!World)
	{
		return EBTNodeResult::Failed;
	}

	// 运行首个EQS并绑定查询完成回调
	UEnvQueryInstanceBlueprintWrapper* Wrapper = UEnvQueryManager::RunEQSQuery(
		World, QueryTemplate, AI.Get(), RunMode, nullptr);
	if (!Wrapper)
	{
		return EBTNodeResult::Failed;
	}
	QueryWrapper = Wrapper;
	Wrapper->GetOnQueryFinishedEvent().AddDynamic(this, &UBTTask_RunEQSAndMove::OnQueryFinished);
	
	return EBTNodeResult::InProgress;
}

void UBTTask_RunEQSAndMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 首次查询完成且尚未发起移动：先判定是否已在范围内，否则发起移动
	if (bQueryFinished && !bMoveRequested && AI.IsValid())
	{
		if (APawn* MyPawn = AI->GetPawn())
		{
			const float DistSq = FVector::DistSquared(Goal, MyPawn->GetActorLocation());
			if (DistSq <= FMath::Square(AcceptableRadius))
			{
				// 已在范围内：直接成功结束
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return;
			}
		}

		// 构造移动请求到当前目标点
		FAIMoveRequest MoveReq(Goal);
		MoveReq.SetAcceptanceRadius(AcceptableRadius);
		MoveReq.SetUsePathfinding(bUsePathfinding);
		MoveReq.SetProjectGoalLocation(bProjectGoalOnNavigation);
		FNavPathSharedPtr Path;
		// 防止重复绑定：先移除旧的，再添加新的
		AI->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_RunEQSAndMove::OnMoveFinished);
		AI->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_RunEQSAndMove::OnMoveFinished);
		MoveRequestId = AI->MoveTo(MoveReq, &Path);
		if (MoveRequestId.IsValid())
		{
			bMoveRequested = true;
		}
		else
		{
			// 找不到路径：任务失败结束
			AI->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_RunEQSAndMove::OnMoveFinished);
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
	// 每帧运行EQS以持续更新目标点
	if (AI.IsValid())
	{
		if (QueryTemplate && AI->GetWorld())
		{
			if (UEnvQueryInstanceBlueprintWrapper* Wrapper =
				UEnvQueryManager::RunEQSQuery(AI->GetWorld(), QueryTemplate, AI.Get(), RunMode, nullptr))
			{
				Wrapper->GetOnQueryFinishedEvent().AddDynamic(this, &UBTTask_RunEQSAndMove::OnQueryFinished);
				QueryWrapper = Wrapper;
			}
		}
	}
}

EBTNodeResult::Type UBTTask_RunEQSAndMove::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 行为树任务被中止：停止移动并解绑查询事件
	if (AI.IsValid())
	{
		AI->StopMovement();
	}
	if (QueryWrapper.IsValid())
	{
		QueryWrapper->GetOnQueryFinishedEvent().RemoveDynamic(this, &UBTTask_RunEQSAndMove::OnQueryFinished);
	}
	return EBTNodeResult::Aborted;
}

void UBTTask_RunEQSAndMove::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* InWrapper, EEnvQueryStatus::Type Status)
{
	// EQS查询失败：不结束任务，等待下一帧继续查询
	if (!InWrapper || Status != EEnvQueryStatus::Success)
	{
		return;
	}
	const TArray<FVector> Locations = InWrapper->GetResultsAsLocations();
	// 无可用点位：不结束任务，等待下一帧继续查询
	if (Locations.Num() == 0)
	{
		return;
	}
	const FVector NewGoal = Locations[0];
	Goal = NewGoal;
	if (AI.IsValid())
	{
		if (APawn* MyPawn = AI->GetPawn())
		{
			const float DistSqToPawn = FVector::DistSquared(Goal, MyPawn->GetActorLocation());
			if (DistSqToPawn <= FMath::Square(AcceptableRadius))
			{
				// 已在范围内：停止移动并成功结束
				AI->StopMovement();
				if (InstanceOwnerComp.IsValid())
				{
					FinishLatentTask(*InstanceOwnerComp.Get(), EBTNodeResult::Succeeded);
				}
				return;
			}
		}
	}
	if (!bQueryFinished)
	{
		// 首次查询仅标记完成，交由Tick触发第一次MoveTo
		bQueryFinished = true;
		return;
	}
	if (AI.IsValid())
	{
		// 刷新移动目标：向最新EQS点移动
		AI->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_RunEQSAndMove::OnMoveFinished);
		FAIMoveRequest MoveReq(Goal);
		MoveReq.SetAcceptanceRadius(AcceptableRadius);
		MoveReq.SetUsePathfinding(bUsePathfinding);
		MoveReq.SetProjectGoalLocation(bProjectGoalOnNavigation);
		FNavPathSharedPtr Path;
		// 防止重复绑定：先移除旧的，再添加新的
		AI->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_RunEQSAndMove::OnMoveFinished);
		AI->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_RunEQSAndMove::OnMoveFinished);
		MoveRequestId = AI->MoveTo(MoveReq, &Path);
		if (!MoveRequestId.IsValid())
		{
			// 找不到路径：任务失败结束
			AI->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_RunEQSAndMove::OnMoveFinished);
			if (InstanceOwnerComp.IsValid())
			{
				FinishLatentTask(*InstanceOwnerComp.Get(), EBTNodeResult::Failed);
			}
		}
	}
}

void UBTTask_RunEQSAndMove::OnMoveFinished(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	// 移动完成回调：仅在到达目标（Success）时结束任务成功
	if (!InstanceOwnerComp.IsValid())
	{
		return;
	}
	if (AI.IsValid())
	{
		AI->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_RunEQSAndMove::OnMoveFinished);
	}
	if (Result == EPathFollowingResult::Success)
	{
		FinishLatentTask(*InstanceOwnerComp.Get(), EBTNodeResult::Succeeded);
	}
}
