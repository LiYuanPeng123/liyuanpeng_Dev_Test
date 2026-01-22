
// EBTNodeResult::Type UUBTTask_RunCameMode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
#include "BehaviorTree/UBTTask_RunCameMode.h"
#include "CancerCameraBehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CancerCameraMode_ThirdPerson.h"
#include "GameFramework/Actor.h"

EBTNodeResult::Type UUBTTask_RunCameMode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* ActorOwner = OwnerComp.GetOwner();
	UCancerCameraBehaviorTreeComponent* CameraComponent = ActorOwner ? ActorOwner->GetComponentByClass<UCancerCameraBehaviorTreeComponent>() : nullptr;
	
	UCancerCameraMode* ModeTemplate = CameraMode.CameraModeInstance ? CameraMode.CameraModeInstance.Get() : nullptr;

	if (CameraComponent && ModeTemplate)
	{
		// 首选 BT 自带黑板；若为空，回退到组件维护的黑板
		UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
		if (!BB && CameraComponent)
		{
			BB = CameraComponent->GetCameraBlackboardComponent();
		}

		if (BB)
		{
			if (UCancerCameraMode_ThirdPerson* ThirdPersonMode = Cast<UCancerCameraMode_ThirdPerson>(ModeTemplate))
			{
				// Actor优先 其次位置 位置是静态锁定 只会在第一次锁定是生效
				if (!LockTargetKey.SelectedKeyName.IsNone())
				{
					if (AActor* LockActor = Cast<AActor>(BB->GetValueAsObject(LockTargetKey.SelectedKeyName)))
					{
						ThirdPersonMode->bEnableLockOn = true;
						ThirdPersonMode->bEnableLockOnLoc = false;
						ThirdPersonMode->LockOnTarget = LockActor;
					}
				}
				else if (bUseLockTargetLocation && !LockTargetLocationKey.SelectedKeyName.IsNone())
				{
					const FVector LockLoc = BB->GetValueAsVector(LockTargetLocationKey.SelectedKeyName);
					ThirdPersonMode->bEnableLockOn = true;
					ThirdPersonMode->bEnableLockOnLoc = true;
					ThirdPersonMode->LockOnTargetLoc = LockLoc;
				}
			}
		}

		CameraComponent->SetDesiredCameraMode(ModeTemplate);
		//不结束任务 避免重复推入摄像机模式
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UUBTTask_RunCameMode::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);
}

FString UUBTTask_RunCameMode::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}
