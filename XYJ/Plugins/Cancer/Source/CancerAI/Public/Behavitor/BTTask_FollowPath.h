#pragma once

#include "CoreMinimal.h"
#include "CancerAITypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FollowPath.generated.h"


class UBlackboardKeyType_Vector;
enum class EPatrolType : uint8;
/**
 *  查询路径
 */
UCLASS(DisplayName="FollowPath")
class CANCERAI_API UBTTask_FollowPath : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="类型")
	EPatrolType PatrolType{EPatrolType::ERandomPoint};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (AllowedTypes = "Vector")
		, DisplayName="路点黑板键")
	FBlackboardKeySelector WaypointKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="路径tag",
		meta=(EditCondition="PatrolType == EPatrolType::EFollowSpline"))
	FGameplayTag PathTag;

	// 搜索中心（可选），未设置则使用角色当前位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (AllowedTypes = "Vector"),
		DisplayName="搜索中心"
		, meta=(EditCondition="PatrolType == EPatrolType::ERandomPoint"))
	FBlackboardKeySelector SearchCenterKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nav | Random", meta=(ClampMin=0.0)
		, meta=(EditCondition="PatrolType == EPatrolType::ERandomPoint"))
	float MinMoveDistance = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nav | Random", meta=(ClampMin=0.0)
		, meta=(EditCondition="PatrolType == EPatrolType::ERandomPoint"))
	float MaxMoveDistance = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nav | Random", meta=(ClampMin=1)
		, meta=(EditCondition="PatrolType == EPatrolType::ERandomPoint"))
	int32 MaxRandomAttempts = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="范围",
		meta=(EditCondition="PatrolType == EPatrolType::ERandomPoint", ClampMin = 100.f))
	float RandomPatrolRadius = 2000;
};
