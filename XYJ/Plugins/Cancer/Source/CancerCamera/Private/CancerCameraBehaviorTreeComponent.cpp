#include "CancerCameraBehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

void UCancerCameraBehaviorTreeComponent::SetCameraBehaviorTree(UBehaviorTree* NewBT)
{
	CameraBehaviorTree = NewBT;

	if (CameraBehaviorTree)
	{
		if (!BTComponent && GetOwner())
		{
			auto Comp = GetOwner()->AddComponentByClass(UBehaviorTreeComponent::StaticClass(), false,
			                                            FTransform::Identity, false);
			BTComponent = Cast<UBehaviorTreeComponent>(Comp);
		}

		// 创建并初始化黑板组件
		if (!BBComponent && GetOwner())
		{
			BBComponent = NewObject<UBlackboardComponent>(GetOwner(), TEXT("CameraBlackboard"));
			BBComponent->RegisterComponent();
		}
		if (BBComponent && CameraBehaviorTree->BlackboardAsset)
		{
			BBComponent->InitializeBlackboard(*CameraBehaviorTree->BlackboardAsset);
		}

		BTComponent->StartTree(*CameraBehaviorTree.Get());
	}
	else if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void UCancerCameraBehaviorTreeComponent::SetDesiredCameraMode(UCancerCameraMode* NewCameraModeTemplate)
{
	PushCameraModeTemplate(NewCameraModeTemplate);
}

void UCancerCameraBehaviorTreeComponent::DrawDebug(UCanvas* Canvas) const
{
	Super::DrawDebug(Canvas);
}

void UCancerCameraBehaviorTreeComponent::BeginPlay()
{
	Super::BeginPlay();
}
