#include "Nodes/CancerStateMachineNode_End.h"

UCancerStateMachineNode_End::UCancerStateMachineNode_End()
{
	NodeName = NSLOCTEXT("CancerStateMachine", "EndNodeName", "End");
}

void UCancerStateMachineNode_End::OnActivated_Implementation(UObject* Context)
{
	Super::OnActivated_Implementation(Context);
	FinishStateMachine();
}
