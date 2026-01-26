
#include "Tasks/GameplayTask_StartStateMachine.h"

#include "Engine/Engine.h"
#include "VisualLogger/VisualLogger.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CancerStateMachineCondition.h"
#include "GameplayTasksComponent.h"

UGameplayTask_StartStateMachine::UGameplayTask_StartStateMachine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bDebugMode = false;
}

UGameplayTask_StartStateMachine* UGameplayTask_StartStateMachine::StartCancerStateMachine(UObject* TaskOwner, UCancerStateMachineData* InData, UObject* ContextOverride, bool bInDebug)
{
	if (!TaskOwner || !InData || InData->EntryEdges.Num() == 0)
	{
		return nullptr;
	}

	TScriptInterface<IGameplayTaskOwnerInterface> ResolvedOwner;
	
	// 自动解析 TaskOwner
	if (TaskOwner->GetClass()->ImplementsInterface(UGameplayTaskOwnerInterface::StaticClass()))
	{
		ResolvedOwner = TaskOwner;
	}
	else if (AActor* Actor = Cast<AActor>(TaskOwner))
	{
		// 如果是 Actor，自动寻找或创建 GameplayTasksComponent
		if (UGameplayTasksComponent* TaskComp = Actor->FindComponentByClass<UGameplayTasksComponent>())
		{
			ResolvedOwner = TaskComp;
		}
		else
		{
			UGameplayTasksComponent* NewComp = NewObject<UGameplayTasksComponent>(Actor);
			NewComp->RegisterComponent();
			ResolvedOwner = NewComp;
		}
	}

	if (!ResolvedOwner.GetInterface())
	{
		UE_LOG(LogTemp, Error, TEXT("RunCancerStateMachine: TaskOwner %s does not implement IGameplayTaskOwnerInterface and is not an Actor."), *TaskOwner->GetName());
		return nullptr;
	}

	UGameplayTask_StartStateMachine* MyTask = NewTask<UGameplayTask_StartStateMachine>(ResolvedOwner);
	if (MyTask)
	{
		MyTask->StateMachineData = InData;
		MyTask->ContextOverride = ContextOverride;
		MyTask->bDebugMode = bInDebug;
	}
	return MyTask;
}

void UGameplayTask_StartStateMachine::Activate()
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
	StateMachineInstance.OnRequestStateSwitch.AddUObject(this, &UGameplayTask_StartStateMachine::SwitchState);

	// 3. 激活全局转换的条件绑定
	for (UCancerStateMachineEdge* Edge : StateMachineInstance.RuntimeGlobalEdges)
	{
		if (Edge)
		{
			for (UCancerStateMachineCondition* Condition : Edge->Conditions)
			{
				if (UCancerStateMachineCondition_Event* EventCond = Cast<UCancerStateMachineCondition_Event>(Condition))
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
		
		UE_LOG(LogTemp, Log, TEXT("CancerSM: State Machine Started. Initial State: [%s]"), *StartNodeInstance->NodeName.ToString());
		SwitchState(StartNodeInstance);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CancerSM: State Machine Failed to find Initial State!"));
		EndTask();
	}
}

void UGameplayTask_StartStateMachine::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (CurrentState)
	{
		StateTime += DeltaTime;
		// 确保 OnUpdate 被调用
		CurrentState->OnUpdate(GetContext(), DeltaTime);
		
		CheckTransitions(false);
	}
}

void UGameplayTask_StartStateMachine::OnDestroy(bool bInOwnerFinished)
{
	// 如果还在运行某个状态，先退出它
	if (CurrentState)
	{
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

void UGameplayTask_StartStateMachine::StopStateMachine()
{
	EndTask();
}

void UGameplayTask_StartStateMachine::SwitchState(UCancerStateMachineNode* NewState)
{
	if (CurrentState == NewState) return;

	UObject* Context = GetContext();

	// 节点从未被进入过，则在此刻初始化
	if (NewState && !NewState->bInitialized)
	{
		NewState->InitializeNode(Context, this, &StateMachineInstance);
	}

	// 退出旧状态
	if (CurrentState)
	{
		UE_LOG(LogTemp, Log, TEXT("CancerSM: Exiting State [%s] (Class: %s)"), *CurrentState->NodeName.ToString(), *CurrentState->GetClass()->GetName());
		
		UnbindCurrentStateTriggers();
		CurrentState->OnDeactivated(Context);
		OnStateExited.Broadcast(CurrentState);
	}

	CurrentState = NewState;
	StateTime = 0.0f;

	// 进入新状态
	if (CurrentState)
	{
		if (bDebugMode)
		{
			FString DebugMsg = FString::Printf(TEXT("CancerSM: Entered State [%s] (Class: %s)"), *CurrentState->NodeName.ToString(), *CurrentState->GetClass()->GetName());
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, DebugMsg);
			UE_LOG(LogTemp, Log, TEXT("%s"), *DebugMsg);
		}

		CurrentState->OnActivated(Context);
		OnStateEntered.Broadcast(CurrentState);
		BindCurrentStateTriggers();
	}
	else
	{
		// 如果进入空状态，视为状态机结束
		OnStateMachineFinished.Broadcast(nullptr);
		EndTask();
	}
}

void UGameplayTask_StartStateMachine::BindCurrentStateTriggers()
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

void UGameplayTask_StartStateMachine::UnbindCurrentStateTriggers()
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

void UGameplayTask_StartStateMachine::CheckTransitions(bool bIsEventTrigger)
{
	UObject* Context = GetContext();

	if (UCancerStateMachineNode* NextState = StateMachineInstance.CheckTransitions(Context, CurrentState, StateTime, bIsEventTrigger))
	{
		SwitchState(NextState);
	}
}

UObject* UGameplayTask_StartStateMachine::GetContext() const
{
	if (ContextOverride)
	{
		return ContextOverride;
	}

	// 优先返回 TaskOwner 关联的 Actor
	if (UObject* Owner = GetOwnerActor())
	{
		return Owner;
	}
	return GetAvatarActor();
}
