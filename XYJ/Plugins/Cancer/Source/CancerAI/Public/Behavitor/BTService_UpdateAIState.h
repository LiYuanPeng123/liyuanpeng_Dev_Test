#pragma once

#include "CoreMinimal.h"
#include "CancerAITypes.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateAIState.generated.h"

/**
 * 
 */
UCLASS(DisplayName="更新AI状态")
class CANCERAI_API UBTService_UpdateAIState : public UBTService
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = Cancer)
	TObjectPtr<class ACancerAIController> AIController;

	UPROPERTY(BlueprintReadOnly, Category = Cancer)
	TObjectPtr<class APawn> CharOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard",DisplayName="默认AI状态")
	EAIState DefaultState = EAIState::EWait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", DisplayName="最大返回距离")
	float MaxDis = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta=(AllowedTypes="Enum"),
		DisplayName="巡路状态")
	FBlackboardKeySelector PatrolType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta=(AllowedTypes="Vector"),
		DisplayName="目标位置")
	FBlackboardKeySelector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta=(AllowedTypes="Float"),
		DisplayName="目标距离")
	FBlackboardKeySelector TargetActorDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta=(AllowedTypes="Float"),
		DisplayName="出生点距离")
	FBlackboardKeySelector SpawnDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta=(AllowedTypes="Vector"),
		DisplayName="出生点位置")
	FBlackboardKeySelector SpawnActorKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta=(AllowedTypes="Enum"),
		DisplayName="AI状态")
	FBlackboardKeySelector AIState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta=(AllowedTypes="Enum"),
		DisplayName="AICombat状态")
	FBlackboardKeySelector AICombatState;

public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;
};
