#include "CancerStateMachineTask.h"

#include "CancerStateMachineNode.h"

UCancerStateMachineTask::UCancerStateMachineTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

void UCancerStateMachineTask::InitializeTask(UObject* InContext)
{
	ContextPtr = InContext;
}

void UCancerStateMachineTask::RegisterTask(IGameplayTaskOwnerInterface& InTaskOwner, uint8 InPriority)
{
	InitTask(InTaskOwner, InPriority);
}

void UCancerStateMachineTask::Activate()
{
	Super::Activate();
	
	UE_LOG(LogTemp, Log, TEXT("CancerSM: Task [%s] Activated"), *GetName());
	BP_OnActivated();
}

void UCancerStateMachineTask::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	
	// UE_LOG(LogTemp, VeryVerbose, TEXT("CancerSM: Task [%s] Tick"), *GetName());
	BP_OnTick(DeltaTime);
}

void UCancerStateMachineTask::OnDestroy(bool bInOwnerFinished)
{
	BP_OnDeactivated();
	Super::OnDestroy(bInOwnerFinished);
}

void UCancerStateMachineTask::FinishNode()
{
	if (UCancerStateMachineNode* OwningNode = Cast<UCancerStateMachineNode>(GetOuter()))
	{
		OwningNode->FinishNode();
	}
	EndTask();
}
