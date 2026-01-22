#include "Behavitor/BTTask_FollowPath.h"
#include "AIController.h"
#include "AIPatrolComponent.h"
#include "CancerAIFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "ParolPathManagerSubsystem.h"
static TAutoConsoleVariable<bool> FollowVar(TEXT("Cancer.FollowPath"), false,TEXT(""));

EBTNodeResult::Type UBTTask_FollowPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;
	AActor* AvatarActor = nullptr;
	if (AAIController* AIOwner = OwnerComp.GetAIOwner())
	{
		AvatarActor = AIOwner->GetPawn();
	}
	else
	{
		AvatarActor = OwnerComp.GetOwner();
	}
	if (!IsValid(AvatarActor))
	{
		return EBTNodeResult::Failed;
	}
	FVector TargetLocation = FVector::ZeroVector;
	if (PatrolType == EPatrolType::ERandomPoint)
	{
		const UBlackboardComponent* BBRead = OwnerComp.GetBlackboardComponent();
		const FVector Origin = (BBRead && SearchCenterKey.SelectedKeyName != NAME_None)
			                       ? BBRead->GetValueAsVector(SearchCenterKey.SelectedKeyName)
			                       : AvatarActor->GetActorLocation();

		if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
		{
			bool bFound = false;
			FNavLocation NavLoc;
			const float Radius = RandomPatrolRadius;
			for (int32 i = 0; i < MaxRandomAttempts; ++i)
			{
				if (NavSys->GetRandomPointInNavigableRadius(Origin, Radius, NavLoc))
				{
					const float Dist = FVector::Dist(Origin, NavLoc.Location);
					if (Dist >= MinMoveDistance && Dist <= MaxMoveDistance)
					{
						TargetLocation = NavLoc.Location;
						bFound = true;
						break;
					}
				}
			}
			if (!bFound)
			{
				return EBTNodeResult::Failed;
			}
		}
	}
	else
	{
		UAIPatrolComponent* PatrolComponent = UCancerAIFunctionLibrary::GetAIPatrolComponent(AvatarActor);
		UParolPathManagerSubsystem* PathSub = GetWorld()->GetSubsystem<UParolPathManagerSubsystem>();
		auto Path = PathSub->GetPatrolPath(PathTag);
		if (!IsValid(PatrolComponent) || !PathTag.IsValid() || !PathSub ||
			!Path)
		{
			return EBTNodeResult::Failed;
		}
		PatrolComponent->SetPathToFollow(Path);
		if (!PatrolComponent->TryGetNextWaypoint(TargetLocation))
		{
			return EBTNodeResult::Failed;
		}
	}

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		if (WaypointKey.SelectedKeyName != NAME_None)
		{
			BB->SetValueAsVector(WaypointKey.SelectedKeyName, TargetLocation);
		}
	}
	return EBTNodeResult::Succeeded;
}
