#include "CancerStateMachineCondition.h"

UCancerStateMachineCondition_DelayTime::UCancerStateMachineCondition_DelayTime()
	: DelayTime(0.0f)
{
}

bool UCancerStateMachineCondition_DelayTime::IsSatisfied_Implementation(
	UObject* Context, const UCancerStateMachineNode* SourceNode, float StateTime) const
{
	if (StateTime < DelayTime) return false;
	return true;
}
