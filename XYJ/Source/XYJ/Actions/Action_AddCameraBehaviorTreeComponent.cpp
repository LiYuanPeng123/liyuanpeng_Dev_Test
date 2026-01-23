#include "Action_AddCameraBehaviorTreeComponent.h"
#include "CancerCameraBehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

void UAction_AddCameraBehaviorTreeComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	if (!CameraBehaviorTree.IsNull())
		OutPaths.Add(CameraBehaviorTree.ToSoftObjectPath());
}

void UAction_AddCameraBehaviorTreeComponent::PostInitComponent_Implementation()
{
	UCancerCameraBehaviorTreeComponent* CameraBehaviorTreeComponent =
		Cast<UCancerCameraBehaviorTreeComponent>(GetCreateComponent());
	check(CameraBehaviorTreeComponent);
}

void UAction_AddCameraBehaviorTreeComponent::FinalizeAfterComponent_Implementation()
{
	Super::FinalizeAfterComponent_Implementation();
	UCancerCameraBehaviorTreeComponent* CameraBehaviorTreeComponent =
		Cast<UCancerCameraBehaviorTreeComponent>(GetCreateComponent());
	check(CameraBehaviorTreeComponent);
	CameraBehaviorTreeComponent->SetCameraBehaviorTree(CameraBehaviorTree.Get());
}
