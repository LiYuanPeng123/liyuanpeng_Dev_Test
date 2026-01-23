#include "CancerAIController.h"

#include "AbilitySystemGlobals.h"
#include "AIPatrolComponent.h"
#include "CancerAITypes.h"
#include "CancerCoreFunctionLibrary.h"
#include "PatrolPath.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Components/CancerCombatAttributeComponent.h"
#include "Components/CancerHeroComponent.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionConfigAsset.h"
#include "Statics/CancerStaticsSubsystem.h"


ACancerAIController::ACancerAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComp"));
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
}


template <typename T>
static T* SafeFindComponent(AActor* Owner)
{
	return Owner ? Owner->FindComponentByClass<T>() : nullptr;
}

TObjectPtr<class UAIPatrolComponent> ACancerAIController::TryGetPatrolComp() const
{
	if (IsValid(patrolComp))
	{
		return patrolComp;
	}
	if (IsValid(CharacterOwned))
	{
		return CharacterOwned->FindComponentByClass<UAIPatrolComponent>();
	}
	return nullptr;
}

void ACancerAIController::SetBehaviorTree(UBehaviorTree* InBehaviorTree)
{
	check(InBehaviorTree);
	BehaviorTree = InBehaviorTree;
}

void ACancerAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ACancerAIController::EndPlay(const EEndPlayReason::Type reason)
{
	Super::EndPlay(reason);
}

void ACancerAIController::StartBehaviorTree()
{
	check(BehaviorTree);
	if (BehaviorTree == nullptr)
	{
		return;
	}
	

	UBlackboardData* const bbData = BehaviorTree->BlackboardAsset;
	if (bbData == nullptr)
	{
		return;
	}

	BlackboardComponent->InitializeBlackboard(*bbData);
	Blackboard = BlackboardComponent;
	targetActorKey = BlackboardComponent->GetKeyID("TargetActor");
	targetPointLocationKey = BlackboardComponent->GetKeyID("TargetLocation");
	targetActorDistanceKey = BlackboardComponent->GetKeyID("TargetActorDistance");
	aiStateKey = BlackboardComponent->GetKeyID("AIState");
	aiCombatStateKey = BlackboardComponent->GetKeyID("AICombatState");
	isPausedKey = BlackboardComponent->GetKeyID("IsPaused");
	homeDistanceKey = BlackboardComponent->GetKeyID("SpawnDistance");
	pathPointKey = BlackboardComponent->GetKeyID("PathPoint");
	SpawnActorLocation = GetPawn()->GetActorLocation();
	if (PerceptionComponent)
	{
		PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ACancerAIController::HandleOnPerceptionUpdated);
		PerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ACancerAIController::HandleOnTargetPerceptionForgotten);
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACancerAIController::HandleOnTargetPerceptionUpdated);
		PerceptionComponent->OnTargetPerceptionInfoUpdated.AddDynamic(this, &ACancerAIController::HandleOnTargetPerceptionInfoUpdated);
	}
	BehaviorTreeComponent->StartTree(*BehaviorTree);
}

float ACancerAIController::GetPathDistanceFromTarget()
{
	return GetPathFollowingComponent()->GetRemainingPathCost();
}


void ACancerAIController::SetTargetActorBK(AActor* target)
{
	if (Blackboard)
		Blackboard->SetValue<UBlackboardKeyType_Object>(targetActorKey, target);
}

AActor* ACancerAIController::GetTargetActorBK() const
{
	if (Blackboard)
	{
		AActor* targetActor = Cast<AActor>(Blackboard->GetValue<UBlackboardKeyType_Object>(targetActorKey));
		return targetActor;
	}
	return nullptr;
}

void ACancerAIController::SetAIStateBK(EAIState state)
{
	if (Blackboard)
		Blackboard->SetValue<UBlackboardKeyType_Enum>(aiStateKey, static_cast<uint8>(state));
}

EAIState ACancerAIController::GetAIStateBK() const
{
	if (Blackboard)
	{
		const uint8 Raw = Blackboard->GetValue<UBlackboardKeyType_Enum>(aiStateKey);
		return static_cast<EAIState>(Raw);
	}
	return EAIState::EWait;
}

void ACancerAIController::SetAICombatStateBK(EAICombatState state)
{
	if (Blackboard)
		Blackboard->SetValue<UBlackboardKeyType_Enum>(aiCombatStateKey, static_cast<uint8>(state));
}

EAICombatState ACancerAIController::GetAICombatStateBK() const
{
	if (Blackboard)
	{
		const uint8 Raw = Blackboard->GetValue<UBlackboardKeyType_Enum>(aiCombatStateKey);
		return static_cast<EAICombatState>(Raw);
	}
	return EAICombatState::EFlee;
}

// 封装黑板：设置当前目标
void ACancerAIController::SetTarget(AActor* target)
{
	SetTargetActorBK(target);
	OnLockTarget.Broadcast(GetPawn(), target);
	if (target == nullptr)
	{
		UCancerStaticsSubsystem::StaticBattleEnd(GetPawn());
	}
	else
	{
		UCancerStaticsSubsystem::StaticBattleStart(GetPawn());
	}
	
}

AActor* ACancerAIController::GetTarget() const
{
	return GetTargetActorBK();
}

bool ACancerAIController::HasTarget() const
{
	return GetTarget() != nullptr;
}

bool ACancerAIController::RequestAnotherTarget(TArray<FCancerQueryMatch> TagQuery)
{
	UAIPerceptionComponent* Perception = PerceptionComponent;
	if (!Perception || !GetPawn())
	{
		return false;
	}
	// 获取所有感知到的角色
	TArray<AActor*> Candidates;
	Perception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), Candidates);
	
	
	TArray<AActor*> FilterActors =  Candidates.FilterByPredicate([this,TagQuery](const AActor* Actor)->bool
	{
		if (UCancerCoreFunctionLibrary::MatchAllQueryByActor(Actor,TagQuery))
		{
			return true;
		}
		return false;
	});
	
	
	if (FilterActors.Num() == 0)
	{
		SetTarget(nullptr);
		return false;
	}
	AActor* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	const FVector MyLoc = GetPawn()->GetActorLocation();
	// 从候选中选择最近的角色（可被配置资产过滤）
	for (AActor* Cand : FilterActors)
	{
		if (!IsValid(Cand) || Cand == GetPawn()) continue;
		if (ActivePerceptionConfig)
		{
			if (!ActivePerceptionConfig->ShouldAcceptTarget(this, Cand))
			{
				continue;
			}
		}
		const float DistSq = FVector::DistSquared(MyLoc, Cand->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = Cand;
		}
	}
	SetTarget(Best);
	if (Best)
	{
		return true;
	}
	return false;
}

void ACancerAIController::OnPossess(APawn* _possPawn)
{
	Super::OnPossess(_possPawn);
	CharacterOwned = Cast<ACharacter>(_possPawn);
	if (!CharacterOwned)
	{
		return;
	}
}

void ACancerAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ACancerAIController::HandleOnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	UE_LOG(LogTemp, Warning, TEXT("ACancerAIController::HandleOnPerceptionUpdated"))
}

void ACancerAIController::HandleOnTargetPerceptionForgotten(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("ACancerAIController::HandleOnTargetPerceptionForgotten"))
}



void ACancerAIController::HandleOnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
 {
	UE_LOG(LogTemp, Warning, TEXT("ACancerAIController::HandleOnTargetPerceptionUpdated"))
	if (Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Actor->GetName());
	}
	if (!Actor || Actor == GetPawn())
	{
		return;
	}
	
	if (!Stimulus.WasSuccessfullySensed())
	{
		SetTarget(nullptr);
		return;
	}

	// 策略：若当前无目标，且通过资产过滤，则设置该感知到的Actor为目标
	if (!HasTarget())
	{
		if (!ActivePerceptionConfig || ActivePerceptionConfig->ShouldAcceptTarget(this, Actor))
		{
			SetTarget(Actor);
		}
	}
}

void ACancerAIController::HandleOnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("ACancerAIController::HandleOnTargetPerceptionInfoUpdated"))
}

