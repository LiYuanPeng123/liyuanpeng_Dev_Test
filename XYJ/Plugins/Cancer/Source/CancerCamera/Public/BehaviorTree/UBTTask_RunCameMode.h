// class UUBTTask_RunCameMode
#pragma once

#include "CoreMinimal.h"
#include "CancerCameraMode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UBTTask_RunCameMode.generated.h"

/**
 * 
 */
UCLASS(DisplayName="运行摄像机模式")
class CANCERCAMERA_API UUBTTask_RunCameMode : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;

	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCancerCameraModeInstanced CameraMode;

	// 从黑板读取锁定目标（Actor）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LockOn")
	struct FBlackboardKeySelector LockTargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LockOn")
	bool bUseLockTargetLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LockOn", meta=(EditCondition="bUseLockTargetLocation"))
	struct FBlackboardKeySelector LockTargetLocationKey;
	
};
