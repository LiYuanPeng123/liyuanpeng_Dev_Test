#include "Nodes/CancerStateMachineEdNodeBase.h"
#include "CancerStateMachineSchema.h"
#include "EdGraph/EdGraphPin.h"

UCancerStateMachineEdNodeBase::UCancerStateMachineEdNodeBase()
{
	//节点可以被重命名
	bCanRenameNode = 1;
}

void UCancerStateMachineEdNodeBase::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UCancerStateMachineSchema::PC_Edge, TEXT("In"));
	CreatePin(EGPD_Output, UCancerStateMachineSchema::PC_Edge, TEXT("Transitions"));
	CreatePin(EGPD_Output, UCancerStateMachineSchema::PC_Edge, TEXT("OnFinished"));
}

UEdGraphPin* UCancerStateMachineEdNodeBase::GetInputPin() const
{
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->Direction == EGPD_Input)
		{
			return Pin;
		}
	}
	return nullptr;
}

UEdGraphPin* UCancerStateMachineEdNodeBase::GetOutputPin() const
{
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->Direction == EGPD_Output && Pin->PinName == TEXT("Transitions"))
		{
			return Pin;
		}
	}
	return nullptr;
}

UEdGraphPin* UCancerStateMachineEdNodeBase::GetFinishPin() const
{
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->Direction == EGPD_Output && Pin->PinName == TEXT("OnFinished"))
		{
			return Pin;
		}
	}
	return nullptr;
}
