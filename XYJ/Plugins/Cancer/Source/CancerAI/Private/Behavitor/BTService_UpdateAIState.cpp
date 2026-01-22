#include "Behavitor/BTService_UpdateAIState.h"

#include "CancerAIController.h"
#include "CancerAITypes.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTService_UpdateAIState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AIController = Cast<ACancerAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (AIController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UStateUpdateBTService get controller failed"));
		return;
	}

	CharOwner = AIController->GetPawn();
	if (CharOwner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPawn() is NULL - UStateUpdateBTService"));
		AIController->Destroy();
		return;
	}


	if (!BB)
	{
		return;
	}
	//同步 初始点 和 出生点距离
	const FVector Home = AIController->GetSpawnActorLocation();
	const float SpawnDis = FVector::Distance(CharOwner->GetActorLocation(), Home);
	if (SpawnActorKey.SelectedKeyName != NAME_None)
	{
		BB->SetValueAsVector(SpawnActorKey.SelectedKeyName, Home);
	}
	if (SpawnDistance.SelectedKeyName != NAME_None)
	{
		BB->SetValueAsFloat(SpawnDistance.SelectedKeyName, SpawnDis);
	}

	//同步目标 位置和目标距离
	if (AActor* Target = AIController->GetTarget())
	{
		const float TargetDist = CharOwner->GetDistanceTo(Target);
		const float LoseDistSq = FMath::Square(AIController->LoseTargetDistance);
		const float DistSq = FVector::DistSquared(CharOwner->GetActorLocation(), Target->GetActorLocation());
		if (TargetActorDistance.SelectedKeyName != NAME_None)
		{
			const float Old = BB->GetValueAsFloat(TargetActorDistance.SelectedKeyName);
			if (!FMath::IsNearlyEqual(Old, TargetDist))
			{
				BB->SetValueAsFloat(TargetActorDistance.SelectedKeyName, TargetDist);
			}
		}
		if (TargetLocation.SelectedKeyName != NAME_None)
		{
			const FVector Old = BB->GetValueAsVector(TargetLocation.SelectedKeyName);
			const FVector NewLoc = Target->GetActorLocation();
			if (!Old.Equals(NewLoc))
			{
				BB->SetValueAsVector(TargetLocation.SelectedKeyName, NewLoc);
			}
		}
	}
	else
	{
		if (TargetActorDistance.SelectedKeyName != NAME_None)
		{
			BB->SetValueAsFloat(TargetActorDistance.SelectedKeyName, 0.f);
		}
	}

	//1)返回初始点
	//2)发现目标进入战斗
	EAIState NewState = DefaultState;
	const float SpawnDistanceSq = FVector::DistSquared(CharOwner->GetActorLocation(), Home);
	const float MaxReturnDisSq = FMath::Square(MaxDis);
	if (SpawnDistanceSq > MaxReturnDisSq)
	{
		NewState = EAIState::EReturnHome;
	}
	else if (AIController->HasTarget())
	{
		NewState = EAIState::EBattle;
	}
	if (AIState.SelectedKeyName != NAME_None)
	{
		BB->SetValueAsEnum(AIState.SelectedKeyName, static_cast<uint8>(NewState));
	}
}

void UBTService_UpdateAIState::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	Super::OnSearchStart(SearchData);
	UBehaviorTreeComponent&  OwnerComp = SearchData.OwnerComp;
	AIController = Cast<ACancerAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (AIController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UStateUpdateBTService get controller failed"));
		return;
	}

	CharOwner = AIController->GetPawn();
	if (CharOwner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPawn() is NULL - UStateUpdateBTService"));
		AIController->Destroy();
		return;
	}
	if (!BB)
	{
		return;
	}
	if (AICombatState.SelectedKeyName != NAME_None)
	{
		EAICombatState NewCombatState = EAICombatState::EFlee;
		BB->SetValueAsEnum(AICombatState.SelectedKeyName, static_cast<uint8>(NewCombatState));
	}
}
