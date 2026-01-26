
#include "Tasks/Abilities/Tasks/AbilityTask_RunCancerStateMachine.h"

#include "CancerStateMachineEdge.h"
#include "Engine/Engine.h"
#include "AbilitySystemComponent.h"
#include "CancerStateMachineCondition.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UAbilityTask_RunCancerStateMachine::UAbilityTask_RunCancerStateMachine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bDebugMode = false;
}

UAbilityTask_RunCancerStateMachine* UAbilityTask_RunCancerStateMachine::RunCancerStateMachine(UGameplayAbility* OwningAbility, UCancerStateMachineData* InData, UObject* ContextOverride, bool bInDebug)
{
	if (!InData || InData->EntryEdges.Num() == 0)
	{
		return nullptr;
	}

	UAbilityTask_RunCancerStateMachine* MyTask = NewAbilityTask<UAbilityTask_RunCancerStateMachine>(OwningAbility);
	if (MyTask)
	{
		MyTask->StateMachineData = InData;
		MyTask->ContextOverride = ContextOverride;
		MyTask->bDebugMode = bInDebug;
	}
	return MyTask;
}

void UAbilityTask_RunCancerStateMachine::Activate()
{
	Super::Activate();

	if (!StateMachineData)
	{
		EndTask();
		return;
	}

	UObject* Context = GetContext();

	// 1. 创建状态机实例 (实例化节点和边，修复连接)
	StateMachineData->CreateInstance(this, StateMachineInstance);

	// 2. 绑定直接跳转请求
	StateMachineInstance.OnRequestStateSwitch.AddUObject(this, &UAbilityTask_RunCancerStateMachine::SwitchState);

	// 3. 激活全局转换的条件绑定
	for (UCancerStateMachineEdge* Edge : StateMachineInstance.RuntimeGlobalEdges)
	{
		if (Edge)
		{
			for (UCancerStateMachineCondition* Condition : Edge->Conditions)
			{
				if ( UCancerStateMachineCondition_Event* EventCond = Cast<UCancerStateMachineCondition_Event>(Condition))
				{
					EventCond->ActivateCondition(Context);
				}
			}
		}
	}

	// 3. 启动状态机
	UCancerStateMachineNode* StartNodeTemplate = StateMachineData->GetStartNode(Context);
	if (StartNodeTemplate)
	{
		UCancerStateMachineNode* StartNodeInstance = StateMachineInstance.GetOrCreateNodeInstance(StartNodeTemplate);
		
		// 启动事件监听子任务
		WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(Ability, FGameplayTag::EmptyTag, nullptr, false, false);
		if (WaitEventTask)
		{
			WaitEventTask->EventReceived.AddDynamic(this, &UAbilityTask_RunCancerStateMachine::HandleEventReceived);
			WaitEventTask->ReadyForActivation();
		}

		SwitchState(StartNodeInstance);
	}
	else
	{
		EndTask();
	}
}

void UAbilityTask_RunCancerStateMachine::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (CurrentState)
	{
		StateTime += DeltaTime;
		
		// UE_LOG(LogTemp, VeryVerbose, TEXT("CancerSM (Ability): State Machine Ticking. Current State: [%s]"), *CurrentState->NodeName.ToString());
		
		CurrentState->OnUpdate(GetContext(), DeltaTime);
		CheckTransitions(false); // Tick 触发，不检查事件边
	}
}

void UAbilityTask_RunCancerStateMachine::OnDestroy(bool bInOwnerFinished)
{
	if (WaitEventTask)
	{
		WaitEventTask->EndTask();
		WaitEventTask = nullptr;
	}

	// 如果还在运行某个状态，先退出它
	if (CurrentState)
	{
		if (bDebugMode)
		{
			UE_LOG(LogTemp, Log, TEXT("CancerSM: Stopping State Machine. Deactivating Current State [%s]"), *CurrentState->NodeName.ToString());
		}
		
		UnbindCurrentStateTriggers();
		UObject* Context = GetContext();
		CurrentState->OnDeactivated(Context);

		OnStateExited.Broadcast(CurrentState);
		CurrentState = nullptr;
	}

	// 解绑全局转换的条件
	UObject* Context = GetContext();
	for (UCancerStateMachineEdge* Edge : StateMachineInstance.RuntimeGlobalEdges)
	{
		if (Edge)
		{
			for (UCancerStateMachineCondition* Condition : Edge->Conditions)
			{
				if (UCancerStateMachineCondition_Event* EventCond = Cast<UCancerStateMachineCondition_Event>(Condition))
				{
					EventCond->DeactivateCondition(Context);
				}
			}
		}
	}
	StateMachineInstance.RuntimeGlobalEdges.Empty();
	StateMachineInstance.OnRequestStateSwitch.RemoveAll(this);

	OnStateMachineFinished.Broadcast(nullptr);
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_RunCancerStateMachine::StopStateMachine()
{
	EndTask();
}

void UAbilityTask_RunCancerStateMachine::SwitchState(UCancerStateMachineNode* NewState)
{
	if (CurrentState == NewState) return;

	UObject* Context = GetContext();

	// 延迟初始化：如果节点从未被进入过，则在此刻初始化
	if (NewState && !NewState->bInitialized)
	{
		NewState->InitializeNode(Context, this, &StateMachineInstance);
	}

	// 退出旧状态
	if (CurrentState)
	{
		UE_LOG(LogTemp, Log, TEXT("CancerSM (Ability): Exiting State [%s] (Class: %s)"), *CurrentState->NodeName.ToString(), *CurrentState->GetClass()->GetName());
		
		UnbindCurrentStateTriggers();

		CurrentState->OnDeactivated(Context);
		OnStateExited.Broadcast(CurrentState);
	}

	CurrentState = NewState;
	StateTime = 0.0f;

	// 进入新状态
	if (CurrentState)
	{
		UE_LOG(LogTemp, Log, TEXT("CancerSM (Ability): Entering State [%s] (Class: %s)"), *CurrentState->NodeName.ToString(), *CurrentState->GetClass()->GetName());

		OnStateEntered.Broadcast(CurrentState);
		CurrentState->OnActivated(Context);
		BindCurrentStateTriggers();
	}
	else
	{
		// 如果进入空状态，视为状态机结束
		OnStateMachineFinished.Broadcast(nullptr);
		EndTask();
	}
}

void UAbilityTask_RunCancerStateMachine::BindCurrentStateTriggers()
{
	if (!CurrentState) return;

	UObject* Context = GetContext();
	for (UCancerStateMachineEdge* Edge : CurrentState->OutgoingEdges)
	{
		if (Edge)
		{
			for (UCancerStateMachineCondition* Condition : Edge->Conditions)
			{
				if (UCancerStateMachineCondition_Event* EventCond = Cast<UCancerStateMachineCondition_Event>(Condition))
				{
					// 激活条件绑定 (现在直接跳转逻辑由 Condition 内部处理)
					EventCond->ActivateCondition(Context);
				}
			}
		}
	}
}

void UAbilityTask_RunCancerStateMachine::UnbindCurrentStateTriggers()
{
	if (!CurrentState) return;

	UObject* Context = GetContext();
	for (UCancerStateMachineEdge* Edge : CurrentState->OutgoingEdges)
	{
		if (Edge)
		{
			for (UCancerStateMachineCondition* Condition : Edge->Conditions)
			{
				if (UCancerStateMachineCondition_Event* EventCond = Cast<UCancerStateMachineCondition_Event>(Condition))
				{
					EventCond->DeactivateCondition(Context);
				}
			}
		}
	}
}

void UAbilityTask_RunCancerStateMachine::HandleEventReceived(FGameplayEventData Payload)
{
	// 1. 广播给蓝图委托 (EventTag, EventData)
	if (EventReceived.IsBound())
	{
		EventReceived.Broadcast(Payload.EventTag, Payload);
	}

	// 2. 通知当前活跃的节点
	if (CurrentState)
	{
		CurrentState->OnEventReceived(Payload.EventTag, Payload);
	}

	// 3. 事件触发的转换现在由 Edge 内部绑定的 Condition 直接处理
}

void UAbilityTask_RunCancerStateMachine::CheckTransitions(bool bIsEventTrigger)
{
	UObject* Context = GetContext();

	if (UCancerStateMachineNode* NextState = StateMachineInstance.CheckTransitions(Context, CurrentState, StateTime, bIsEventTrigger))
	{
		SwitchState(NextState);
	}
}

UObject* UAbilityTask_RunCancerStateMachine::GetContext() const
{
	if (ContextOverride)
	{
		return ContextOverride;
	}
	return GetAvatarActor();
}
