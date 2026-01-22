#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTTask_RunEQSAndMove.generated.h"

class UEnvQuery;

UCLASS(DisplayName="RunEQSAndMove")
class CANCERAI_API UBTTask_RunEQSAndMove : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RunEQSAndMove();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category="EQS")
	UEnvQuery* QueryTemplate;
	UPROPERTY(EditAnywhere, Category="EQS")
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode;
	UPROPERTY(EditAnywhere, Category="Move")
	float AcceptableRadius;
	UPROPERTY(EditAnywhere, Category="Move")
	bool bUsePathfinding;
	UPROPERTY(EditAnywhere, Category="Move")
	bool bProjectGoalOnNavigation;

private:
	TWeakObjectPtr<UBehaviorTreeComponent> InstanceOwnerComp;
	TWeakObjectPtr<class UEnvQueryInstanceBlueprintWrapper> QueryWrapper;
	TWeakObjectPtr<class AAIController> AI;
	struct FAIRequestID MoveRequestId;
	bool bQueryFinished{false};
	bool bMoveRequested{false};
	FVector Goal{ForceInit};
	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	UFUNCTION()
	void OnMoveFinished(struct FAIRequestID RequestID, enum EPathFollowingResult::Type Result);
};
