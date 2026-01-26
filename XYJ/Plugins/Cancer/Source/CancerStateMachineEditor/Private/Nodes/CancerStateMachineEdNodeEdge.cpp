#include "Nodes/CancerStateMachineEdNodeEdge.h"
#include "Settings/CancerStateMachineEditorSettings.h"
#include "Nodes/CancerStateMachineEdNode.h"
#include "CancerStateMachineSchema.h"
#include "CancerStateMachineCondition.h"
#include "EdGraph/EdGraphPin.h"

#define LOCTEXT_NAMESPACE "CancerStateMachineEdNodeEdge"

//////////////////////////////////////////////////////////////////////////
// UCancerStateMachineEdNodeEdgeBase

UCancerStateMachineEdNodeEdgeBase::UCancerStateMachineEdNodeEdgeBase()
{
	bCanRenameNode = 0;
}

void UCancerStateMachineEdNodeEdgeBase::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UCancerStateMachineSchema::PC_Edge, TEXT("In"));
	CreatePin(EGPD_Output, UCancerStateMachineSchema::PC_Edge, TEXT("Out"));
}

UEdGraphPin* UCancerStateMachineEdNodeEdgeBase::GetOutputPin() const
{
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->Direction == EGPD_Output && Pin->PinName == TEXT("Out"))
		{
			return Pin;
		}
	}
	return nullptr;
}

UCancerStateMachineEdNode* UCancerStateMachineEdNodeEdgeBase::GetStartNode() const
{
	if (Pins.Num() > 0 && Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<UCancerStateMachineEdNode>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	return nullptr;
}

UCancerStateMachineEdNode* UCancerStateMachineEdNodeEdgeBase::GetEndNode() const
{
	if (Pins.Num() > 1 && Pins[1]->LinkedTo.Num() > 0)
	{
		return Cast<UCancerStateMachineEdNode>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	return nullptr;
}

void UCancerStateMachineEdNodeEdgeBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCancerStateMachineEdNodeEdgeBase, RuntimeEdge))
	{
		SyncEditorConditions();
	}
	else
	{
		SyncRuntimeConditions();
	}
}

//////////////////////////////////////////////////////////////////////////
// UCancerStateMachineEdNodeEdge

UCancerStateMachineEdNodeEdge::UCancerStateMachineEdNodeEdge()
{
}

void UCancerStateMachineEdNodeEdge::SyncRuntimeConditions()
{
	if (RuntimeEdge)
	{
		RuntimeEdge->Conditions = Conditions;
		RuntimeEdge->bIsEventEdge = false;
		RuntimeEdge->Priority = Priority;
		RuntimeEdge->bIsFinishEdge = bIsFinishEdge;
	}
}

void UCancerStateMachineEdNodeEdge::SyncEditorConditions()
{
	if (RuntimeEdge)
	{
		Conditions = RuntimeEdge->Conditions;
		Priority = RuntimeEdge->Priority;
		bIsFinishEdge = RuntimeEdge->bIsFinishEdge;
	}
}

FText UCancerStateMachineEdNodeEdge::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (bIsFinishEdge)
	{
		return LOCTEXT("EdgeNodeTitleFinished", "节点完成转换 (Auto)");
	}
	return LOCTEXT("EdgeNodeTitle", "条件转换 (Tick)");
}

FLinearColor UCancerStateMachineEdNodeEdge::GetNodeTitleColor() const
{
	return UCancerStateMachineEditorSettings::Get()->ConditionEdgeColor;
}

//////////////////////////////////////////////////////////////////////////
// UCancerStateMachineEdNodeEventEdge

UCancerStateMachineEdNodeEventEdge::UCancerStateMachineEdNodeEventEdge()
{
}

void UCancerStateMachineEdNodeEventEdge::SyncRuntimeConditions()
{
	if (RuntimeEdge)
	{
		RuntimeEdge->Conditions = Conditions;
		RuntimeEdge->bIsEventEdge = true;
		RuntimeEdge->Priority = Priority;
		RuntimeEdge->bIsFinishEdge = bIsFinishEdge;
	}
}

void UCancerStateMachineEdNodeEventEdge::SyncEditorConditions()
{
	if (RuntimeEdge)
	{
		Conditions = RuntimeEdge->Conditions;
		Priority = RuntimeEdge->Priority;
		bIsFinishEdge = RuntimeEdge->bIsFinishEdge;
	}
}

FText UCancerStateMachineEdNodeEventEdge::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("EventEdgeNodeTitle", "事件触发 (Event)");
}

FLinearColor UCancerStateMachineEdNodeEventEdge::GetNodeTitleColor() const
{
	return UCancerStateMachineEditorSettings::Get()->EventEdgeColor;
}

#undef LOCTEXT_NAMESPACE
