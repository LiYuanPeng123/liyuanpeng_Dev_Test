#include "Nodes/CancerStateMachineNode_SubStateMachine.h"
#include "CancerStateMachineEdge.h"
#include "CancerStateMachineCondition.h"
#include "GameplayTask.h"
#include "Tasks/Abilities/Tasks/AbilityTask_StartCancerStateMachine.h"
#include "Tasks/GameplayTask_StartStateMachine.h"

UCancerStateMachineNode_SubStateMachine::UCancerStateMachineNode_SubStateMachine()
{
	NodeName = FText::FromString(TEXT("Sub State Machine"));
	SubStateTime = 0.0f;
	bIsWaitingForFinishTransition = false;
}

void UCancerStateMachineNode_SubStateMachine::OnActivated_Implementation(UObject* Context)
{
	Super::OnActivated_Implementation(Context);

	if (!SubStateMachineData)
	{
		UE_LOG(LogTemp, Warning, TEXT("CancerSM: SubStateMachineNode [%s] has no data asset!"), *NodeName.ToString());
		return;
	}

	bIsWaitingForFinishTransition = false;

	// 1. 创建子状态机实例
	SubStateMachineData->CreateInstance(this, SubStateMachineInstance);
	
	// 2. 绑定结束委托
	SubStateMachineInstance.OnStateMachineFinished.AddUObject(this, &UCancerStateMachineNode_SubStateMachine::HandleSubStateMachineFinished);

	// 3. 绑定直接跳转请求
	SubStateMachineInstance.OnRequestStateSwitch.AddUObject(this, &UCancerStateMachineNode_SubStateMachine::HandleRequestSubStateSwitch);

	// 4. 激活子状态机的全局转换触发器
	for (UCancerStateMachineEdge* Edge : SubStateMachineInstance.RuntimeGlobalEdges)
	{
		if (Edge)
		{
			for (UCancerStateMachineCondition* Condition : Edge->Conditions)
			{
				if (UCancerStateMachineCondition_Event* EventCond = Cast<UCancerStateMachineCondition_Event>(Condition))
				{
					
				}
			}
		}
	}

	// 5. 启动子状态机
	UCancerStateMachineNode* StartNodeTemplate = SubStateMachineData->GetStartNode(Context);
	if (StartNodeTemplate)
	{
		UCancerStateMachineNode* StartNodeInstance = SubStateMachineInstance.GetOrCreateNodeInstance(StartNodeTemplate);
		SwitchSubState(Context, StartNodeInstance);
	}
}

void UCancerStateMachineNode_SubStateMachine::OnDeactivated_Implementation(UObject* Context)
{
	// 1. 退出子状态
	if (CurrentSubState)
	{
		UnbindSubStateTriggers(Context);
		CurrentSubState->OnDeactivated(Context);
		CurrentSubState = nullptr;
	}

	// 2. 解绑结束委托
	SubStateMachineInstance.OnStateMachineFinished.RemoveAll(this);
	SubStateMachineInstance.OnRequestStateSwitch.RemoveAll(this);

	// 3. 解绑全局转换触发器
	for (UCancerStateMachineEdge* Edge : SubStateMachineInstance.RuntimeGlobalEdges)
	{
		if (Edge)
		{
			for (UCancerStateMachineCondition* Condition : Edge->Conditions)
			{
				if (UCancerStateMachineCondition_Event* EventCond = Cast<UCancerStateMachineCondition_Event>(Condition))
				{
					
				}
			}
		}
	}

	Super::OnDeactivated_Implementation(Context);
}

void UCancerStateMachineNode_SubStateMachine::OnUpdate_Implementation(UObject* Context, float DeltaTime)
{
	Super::OnUpdate_Implementation(Context, DeltaTime);

	if (CurrentSubState)
	{
		SubStateTime += DeltaTime;
		CurrentSubState->OnUpdate(Context, DeltaTime);
		
		// 检查子状态机内部转换
		CheckSubTransitions(Context);
	}
}

void UCancerStateMachineNode_SubStateMachine::SwitchSubState(UObject* Context, UCancerStateMachineNode* NewState)
{
	if (CurrentSubState == NewState) return;

	// 退出旧子状态
	if (CurrentSubState)
	{
		UnbindSubStateTriggers(Context);
		CurrentSubState->OnDeactivated(Context);
	}

	CurrentSubState = NewState;
	SubStateTime = 0.0f;

	// 进入新子状态
	if (CurrentSubState)
	{
		// 延迟初始化子状态节点
		if (!CurrentSubState->bInitialized)
		{
			CurrentSubState->InitializeNode(Context, OwningTask, &SubStateMachineInstance);
		}

		CurrentSubState->OnActivated(Context);
		BindSubStateTriggers(Context);
	}
}

void UCancerStateMachineNode_SubStateMachine::CheckSubTransitions(UObject* Context)
{
	if (UCancerStateMachineNode* NextState = SubStateMachineInstance.CheckTransitions(Context, CurrentSubState, SubStateTime))
	{
		SwitchSubState(Context, NextState);
	}
}

void UCancerStateMachineNode_SubStateMachine::BindSubStateTriggers(UObject* Context)
{
	if (!CurrentSubState) return;

	for (UCancerStateMachineEdge* Edge : CurrentSubState->OutgoingEdges)
	{
		if (Edge)
		{
			for (UCancerStateMachineCondition* Condition : Edge->Conditions)
			{
				if (UCancerStateMachineCondition_Event* EventCond = Cast<UCancerStateMachineCondition_Event>(Condition))
				{
					
				}
			}
		}
	}
}

void UCancerStateMachineNode_SubStateMachine::UnbindSubStateTriggers(UObject* Context)
{
	if (!CurrentSubState) return;

	for (UCancerStateMachineEdge* Edge : CurrentSubState->OutgoingEdges)
	{
		if (Edge)
		{
			for (UCancerStateMachineCondition* Condition : Edge->Conditions)
			{
				if (UCancerStateMachineCondition_Event* EventCond = Cast<UCancerStateMachineCondition_Event>(Condition))
				{
					
				}
			}
		}
	}
}

void UCancerStateMachineNode_SubStateMachine::HandleSubStateMachineFinished()
{
	// 触发节点完成信号
	FinishNode();
}

void UCancerStateMachineNode_SubStateMachine::HandleRequestSubStateSwitch(UCancerStateMachineNode* NewState)
{
	SwitchSubState(GetContext(), NewState);
}

UObject* UCancerStateMachineNode_SubStateMachine::GetContext() const
{
	if (OwningTask)
	{
		if (UAbilityTask_StartCancerStateMachine* AT = Cast<UAbilityTask_StartCancerStateMachine>(OwningTask))
		{
			return AT->GetAvatarActor();
		}
	}

	if (TaskOwner.GetInterface())
	{
		return Cast<UObject>(TaskOwner.GetInterface());
	}
	return nullptr;
}
