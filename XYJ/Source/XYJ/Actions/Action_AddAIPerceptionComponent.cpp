

#include "Action_AddAIPerceptionComponent.h"
#include "CancerAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"

void UAction_AddAIPerceptionComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	if (!BehaviorTree.IsNull())
	OutPaths.Add(BehaviorTree.ToSoftObjectPath());
	if (!PerceptionConfig.IsNull())
		OutPaths.Add(PerceptionConfig.ToSoftObjectPath());
}

void UAction_AddAIPerceptionComponent::PostInitComponent_Implementation()
{
    if (ACharacter* Character = Cast<ACharacter>( GetCreatActor()))
    {
        if (ACancerAIController* AIController = Cast<ACancerAIController>(Character->GetController()))
        {
            AIController->SetActivePerceptionConfig(PerceptionConfig.Get());
            PerceptionConfig->ApplyToController(AIController);
        	AIController->SetBehaviorTree(BehaviorTree.Get());
        	UE_LOG(LogTemp, Log, TEXT("已设置默认行为树: %s"), *BehaviorTree->GetName());
        	
        }
    }
}

void UAction_AddAIPerceptionComponent::FinalizeAfterComponent_Implementation()
{
	Super::FinalizeAfterComponent_Implementation();

	if (ACharacter* Character = Cast<ACharacter>( GetCreatActor()))
	{
		if (ACancerAIController* AIController = Cast<ACancerAIController>(Character->GetController()))
		{
			AIController->StartBehaviorTree();
		}
	}
	
}
