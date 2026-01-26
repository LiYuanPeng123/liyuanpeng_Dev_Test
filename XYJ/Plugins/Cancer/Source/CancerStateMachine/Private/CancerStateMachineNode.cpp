#include "CancerStateMachineNode.h"
#include "CancerStateMachineData.h"
#include "CancerStateMachineTask.h"
#include "CancerStateMachineEdge.h"
#include "GameplayTagAssetInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

UCancerStateMachineNode::UCancerStateMachineNode()
	: bInitialized(false)
{
	NodeName = FText::FromString(TEXT("New State"));
}

void UCancerStateMachineNode::InitializeNode(UObject* Context, UGameplayTask* InOwningTask, FCancerStateMachineInstance* InParentInstance)
{
	if (bInitialized) return;

	OwningTask = InOwningTask;
	ParentInstance = InParentInstance;

	// 设置 TaskOwner
	if (OwningTask)
	{
		if (IGameplayTaskOwnerInterface* OwnerInterface = OwningTask->GetTaskOwner())
		{
			UObject* OwnerObj = Cast<UObject>(OwnerInterface);
			TaskOwner.SetObject(OwnerObj);
			TaskOwner.SetInterface(OwnerInterface);
		}
	}

	OnInitialized(Context);
	bInitialized = true;
}

void UCancerStateMachineNode::HandleStateTag(UObject* Context, bool bAdd)
{
	if (!StateTag.IsValid()) return;

	AActor* ContextActor = Cast<AActor>(Context);
	if (!ContextActor) return;

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ContextActor))
	{
		if (bAdd)
		{
			ASC->AddLooseGameplayTag(StateTag);
		}
		else
		{
			ASC->RemoveLooseGameplayTag(StateTag);
		}
	}
}


UCancerStateMachineNode_Task::UCancerStateMachineNode_Task()
{
}

void UCancerStateMachineNode::FinishNode()
{
	bIsNodeFinished = true;
	OnNodeCompleted.Broadcast();
}

void UCancerStateMachineNode::FinishStateMachine()
{
	// 1. 如果有父实例（说明是在子状态机中），触发父实例的结束信号
	if (ParentInstance)
	{
		ParentInstance->OnStateMachineFinished.Broadcast();
		return;
	}

	// 2. 否则结束整个任务
	if (OwningTask)
	{
		OwningTask->EndTask();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CancerSM: Cannot FinishStateMachine, OwningTask is null on node [%s]"), *NodeName.ToString());
	}
}

void UCancerStateMachineNode::OnActivated_Implementation(UObject* Context)
{
	bIsNodeFinished = false;
	HandleStateTag(Context, true);
}

void UCancerStateMachineNode::OnDeactivated_Implementation(UObject* Context)
{
	HandleStateTag(Context, false);
}

void UCancerStateMachineNode_Task::OnActivated_Implementation(UObject* Context)
{
	Super::OnActivated_Implementation(Context);

	// 启动所有配置的任务
	ActiveTaskInstances.Empty();
	for (UCancerStateMachineTask* TaskTemplate : StateTasks)
	{
		if (TaskTemplate)
		{
			// 实例化任务 (因为 UGameplayTask 需要在运行时独立运行)
			UCancerStateMachineTask* TaskInstance = DuplicateObject<UCancerStateMachineTask>(TaskTemplate, this);
			if (TaskInstance)
			{
				if (TaskOwner.GetInterface())
				{
					// 使用公开的 RegisterTask 绕过 InitTask 的访问限制
					TaskInstance->RegisterTask(*TaskOwner, TaskInstance->GetPriority());
				}
				
				TaskInstance->InitializeTask(Context);
				TaskInstance->ReadyForActivation();
				ActiveTaskInstances.Add(TaskInstance);
			}
		}
	}
}

void UCancerStateMachineNode_Task::OnDeactivated_Implementation(UObject* Context)
{
	// 结束所有运行中的任务
	for (UCancerStateMachineTask* TaskInstance : ActiveTaskInstances)
	{
		if (TaskInstance)
		{
			TaskInstance->EndTask();
		}
	}
	ActiveTaskInstances.Empty();

	Super::OnDeactivated_Implementation(Context);
}
