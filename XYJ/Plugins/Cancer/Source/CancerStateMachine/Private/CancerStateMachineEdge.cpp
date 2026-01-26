#include "CancerStateMachineEdge.h"
#include "CancerStateMachineNode.h"
#include "Nodes/CancerStateMachineNode_SubStateMachine.h"
#include "CancerStateMachineCondition.h"
#include "CancerStateMachineData.h"

UCancerStateMachineEdge::UCancerStateMachineEdge()
{
}

bool UCancerStateMachineEdge::CanTransition(UObject* Context, float StateTime) const
{
	return CheckConditions(Context, StateTime);
}

void UCancerStateMachineEdge::HandleEventTriggered(UObject* Context)
{
	if (OwningInstance)
	{
		OwningInstance->HandleEventTriggered(Context, this);
	}
}

bool UCancerStateMachineEdge::CheckConditions(UObject* Context, float StateTime) const
{
	// 1. 检查逻辑条件
	for (auto Condition : Conditions)
	{
		// 只有 Tick 类型的条件需要在轮询中检查
		if (UCancerStateMachineCondition_Tick* TickCond = Cast<UCancerStateMachineCondition_Tick>(Condition))
		{
			if (!TickCond->IsSatisfied(Context, StartNode, StateTime))
			{
				return false;
			}
		}
	}

	return true;
}
