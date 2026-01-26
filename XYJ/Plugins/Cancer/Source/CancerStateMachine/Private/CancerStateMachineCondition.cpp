#include "CancerStateMachineCondition.h"
#include "CancerStateMachineEdge.h"
#include "CancerStateMachineNode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

UCancerStateMachineCondition_TimeWindow::UCancerStateMachineCondition_TimeWindow()
	: StartTime(0.0f)
	, EndTime(0.0f)
{
}

bool UCancerStateMachineCondition_TimeWindow::IsSatisfied_Implementation(UObject* Context, const UCancerStateMachineNode* SourceNode, float StateTime) const
{
	if (StartTime > 0.0f && StateTime < StartTime) return false;
	if (EndTime > 0.0f && StateTime > EndTime) return false;
	return true;
}

void UCancerStateMachineCondition_TransitionInput::ActivateCondition_Implementation(UObject* Context)
{
	if (!InputAction || !Context) return;

	AActor* Actor = Cast<AActor>(Context);
	if (!Actor) return;

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(Actor->InputComponent))
	{
		BindingHandle = EnhancedInputComponent->BindAction(InputAction, TriggerEvent, this, &UCancerStateMachineCondition_TransitionInput::HandleInputAction).GetHandle();
	}
}

void UCancerStateMachineCondition_TransitionInput::DeactivateCondition_Implementation(UObject* Context)
{
	if (!Context) return;

	AActor* Actor = Cast<AActor>(Context);
	if (!Actor) return;

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(Actor->InputComponent))
	{
		EnhancedInputComponent->RemoveBindingByHandle(BindingHandle);
	}
}

void UCancerStateMachineCondition_TransitionInput::HandleInputAction()
{
	// 这里我们需要传回 Context。但是 HandleInputAction 是由 EnhancedInput 调用的，没有 Context。
	// 在 ActivateCondition 中我们知道 Actor 是 Context。
	// 实际上，在状态机任务中，HandleConditionTriggered 的 Context 就是任务运行的 Actor。
	// 所以我们可以直接广播，让任务自己获取 Context。
	// 但 BroadcastTriggered 需要一个 UObject* Context。
	
	// 由于这个 Condition 是 Instanced 的，每个 Node/Edge 都有自己的实例。
	// 但在运行时，它是从模板复制的。
	// 我们需要确保在 Broadcast 时能拿到正确的 Context。
	
	// 我们可以暂时从 InputActionInstance 拿到 Actor，或者在 Activate 时保存它。
	// 但由于 Condition 是 UObject，保存 Actor 指针需要注意 GC。
	
	// 实际上，BroadcastTriggered(nullptr) 也可以，只要接收方知道如何处理。
	// 或者我们从当前 GWorld 拿到第一个玩家的 Pawn。
	BroadcastTriggered(nullptr); 
}

void UCancerStateMachineCondition_Event::BroadcastTriggered(UObject* Context)
{
	if (OwningEdge)
	{
		OwningEdge->HandleEventTriggered(Context);
	}
	OnConditionTriggered.ExecuteIfBound(Context);
}
