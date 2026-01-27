#include "Tasks/Abilities/Tasks/AbilityTask_StartCancerStateMachine.h"

#include "CancerStateMachineEdge.h"
#include "Engine/Engine.h"
#include "AbilitySystemComponent.h"
#include "CancerStateMachineCondition.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UAbilityTask_StartCancerStateMachine::UAbilityTask_StartCancerStateMachine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = 1;
	bDebugMode = false;
}

UAbilityTask_StartCancerStateMachine* UAbilityTask_StartCancerStateMachine::StartCancerStateMachine(
	UGameplayAbility* OwningAbility,
	UCancerStateMachineData* InData, bool bInDebug)
{
	if (!InData || InData->EntryEdges.Num() == 0)
	{
		return nullptr;
	}

	UAbilityTask_StartCancerStateMachine* MyTask = NewAbilityTask<UAbilityTask_StartCancerStateMachine>(OwningAbility);
	if (MyTask)
	{
		MyTask->StateMachineData = InData;
		MyTask->bDebugMode = bInDebug;
	}
	return MyTask;
}

void UAbilityTask_StartCancerStateMachine::Activate()
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
	StateMachineInstance.OnRequestStateSwitch.AddUObject(this, &UAbilityTask_StartCancerStateMachine::SwitchState);

	// 3. 启动状态机
	UCancerStateMachineNode* StartNodeTemplate = StateMachineData->GetStartNode(Context);
	if (StartNodeTemplate)
	{
		UCancerStateMachineNode* StartNodeInstance = StateMachineInstance.GetOrCreateNodeInstance(StartNodeTemplate);

		// 启动事件监听子任务
		WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			Ability, FGameplayTag::EmptyTag, nullptr, false, false);
		if (WaitEventTask)
		{
			WaitEventTask->EventReceived.AddDynamic(this, &UAbilityTask_StartCancerStateMachine::HandleEventReceived);
			WaitEventTask->ReadyForActivation();
		}

		SwitchState(StartNodeInstance);
	}
	else
	{
		EndTask();
	}
}

void UAbilityTask_StartCancerStateMachine::TickTask(float DeltaTime)
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

void UAbilityTask_StartCancerStateMachine::OnDestroy(bool bInOwnerFinished)
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
			UE_LOG(LogTemp, Log, TEXT("CancerSM: Stopping State Machine. Deactivating Current State [%s]"),
			       *CurrentState->NodeName.ToString());
		}

		UnbindCurrentStateTriggers();
		UObject* Context = GetContext();
		CurrentState->OnDeactivated(Context);

		OnStateExited.Broadcast(CurrentState);
		CurrentState = nullptr;
	}

	// 解绑全局转换的条件
	StateMachineInstance.RuntimeGlobalEdges.Empty();
	StateMachineInstance.OnRequestStateSwitch.RemoveAll(this);

	OnStateMachineFinished.Broadcast(nullptr);
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_StartCancerStateMachine::StopStateMachine()
{
	EndTask();
}

void UAbilityTask_StartCancerStateMachine::SwitchState(UCancerStateMachineNode* NewState)
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
		UE_LOG(LogTemp, Log, TEXT("CancerSM (Ability): Exiting State [%s] (Class: %s)"),
		       *CurrentState->NodeName.ToString(), *CurrentState->GetClass()->GetName());

		UnbindCurrentStateTriggers();

		CurrentState->OnDeactivated(Context);
		OnStateExited.Broadcast(CurrentState);
	}

	CurrentState = NewState;
	StateTime = 0.0f;

	// 进入新状态
	if (CurrentState)
	{
		UE_LOG(LogTemp, Log, TEXT("CancerSM (Ability): Entering State [%s] (Class: %s)"),
		       *CurrentState->NodeName.ToString(), *CurrentState->GetClass()->GetName());

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

void UAbilityTask_StartCancerStateMachine::BindCurrentStateTriggers()
{
	// 事件条件绑定现在是隐式的，由 HandleEventReceived 处理
}

void UAbilityTask_StartCancerStateMachine::UnbindCurrentStateTriggers()
{
	// 事件条件绑定现在是隐式的，由 HandleEventReceived 处理
}

void UAbilityTask_StartCancerStateMachine::HandleEventReceived(FGameplayEventData Payload)
{
	// 1. 常规广播和节点通知
	if (EventReceived.IsBound()) EventReceived.Broadcast(Payload.EventTag, Payload);
	if (CurrentState) CurrentState->OnEventReceived(Payload.EventTag, Payload);
	
	// 2. 统一处理转换逻辑
	if (!CurrentState) return;

	// 优先检查全局转换
	for (UCancerStateMachineEdge* Edge : StateMachineInstance.RuntimeGlobalEdges)
	{
		if (!Edge) continue;

		for (UCancerStateMachineCondition* Condition : Edge->Conditions)
		{
			if (UCancerStateMachineCondition_Event* EventCond = Cast<UCancerStateMachineCondition_Event>(Condition))
			{
				bool bMatch = EventCond->bExactMatch
								  ? Payload.EventTag.MatchesTagExact(EventCond->TriggerTag)
								  : Payload.EventTag.MatchesTag(EventCond->TriggerTag);
				if (bMatch)
				{
					if (bDebugMode)
					{
						UE_LOG(LogTemp, Log, TEXT("CancerSM: Global Event [%s] matched. Transitioning..."),
							   *Payload.EventTag.ToString());
					}
					SwitchState(Edge->EndNode);
					return;
				}
			}
		}
	}

	// 检查当前状态转换
	for (UCancerStateMachineEdge* Edge : CurrentState->OutgoingEdges)
	{
		if (!Edge) continue;

		for (UCancerStateMachineCondition* Condition : Edge->Conditions)
		{
			if (UCancerStateMachineCondition_Event* EventCond = Cast<UCancerStateMachineCondition_Event>(Condition))
			{
				bool bMatch = EventCond->bExactMatch
					              ? Payload.EventTag.MatchesTagExact(EventCond->TriggerTag)
					              : Payload.EventTag.MatchesTag(EventCond->TriggerTag);
				if (bMatch)
				{
					// 只要匹配成功，立刻触发跳转
					if (bDebugMode)
					{
						UE_LOG(LogTemp, Log, TEXT("CancerSM: Event [%s] matched. Transitioning..."),
						       *Payload.EventTag.ToString());
					}
					SwitchState(Edge->EndNode);
					return;
				}
			}
		}
	}
}

void UAbilityTask_StartCancerStateMachine::CheckTransitions(bool bIsEventTrigger)
{
	UObject* Context = GetContext();

	if (UCancerStateMachineNode* NextState = StateMachineInstance.CheckTransitions(
		Context, CurrentState, StateTime, bIsEventTrigger))
	{
		SwitchState(NextState);
	}
}

UObject* UAbilityTask_StartCancerStateMachine::GetContext() const
{
	return GetAvatarActor();
}
