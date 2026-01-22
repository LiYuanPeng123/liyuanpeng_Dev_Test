#include "ComboGraphSchema.h"
#include "ComboGraphSchemaActions.h"
#include "ComboGraphEdNode_Move.h"
#include "ComboGraphEdNode_Transition.h"
#include "ComboGraphEdNode_Entry.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"

void UComboGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TSharedPtr<FComboGraphSchemaAction_NewMove> NewMove(new FComboGraphSchemaAction_NewMove());
	ContextMenuBuilder.AddAction(NewMove);
	TSharedPtr<FComboGraphSchemaAction_NewTransition> NewTransition(new FComboGraphSchemaAction_NewTransition());
	ContextMenuBuilder.AddAction(NewTransition);
}

const FPinConnectionResponse UComboGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (!A || !B) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid pins"));
	if (A == B) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Same pin"));
	if (A->GetOwningNode() == B->GetOwningNode()) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Same node"));
	if (A->Direction != EGPD_Output || B->Direction != EGPD_Input)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Wrong pin directions"));
	}
	if (A->PinType.PinCategory != TEXT("Combo") || B->PinType.PinCategory != TEXT("Combo"))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Unsupported pin category"));
	}
	// Allow Entry->Move; will auto insert Transition in TryCreateConnection
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

bool UComboGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	const FPinConnectionResponse Response = CanCreateConnection(A, B);
	if (Response.Response != CONNECT_RESPONSE_MAKE)
	{
		return false;
	}
	UEdGraphNode* NodeA = A->GetOwningNode();
	UEdGraphNode* NodeB = B->GetOwningNode();
	if (!NodeA || !NodeB) return false;
	UEdGraph* ParentGraph = NodeA->GetGraph();
	if (!ParentGraph || ParentGraph != NodeB->GetGraph()) return false;

	UComboGraphEdNode_Move* MoveA = Cast<UComboGraphEdNode_Move>(NodeA);
	UComboGraphEdNode_Move* MoveB = Cast<UComboGraphEdNode_Move>(NodeB);
	UComboGraphEdNode_Transition* TrA = Cast<UComboGraphEdNode_Transition>(NodeA);
	UComboGraphEdNode_Transition* TrB = Cast<UComboGraphEdNode_Transition>(NodeB);
	UComboGraphEdNode_Entry* EntryA = Cast<UComboGraphEdNode_Entry>(NodeA);
	UComboGraphEdNode_Entry* EntryB = Cast<UComboGraphEdNode_Entry>(NodeB);
	if (EntryA && MoveB)
	{
		UComboGraphEdNode_Transition* Transition = NewObject<UComboGraphEdNode_Transition>(ParentGraph, UComboGraphEdNode_Transition::StaticClass(), NAME_None, RF_Transactional);
		Transition->From = nullptr;
		Transition->To = MoveB->Move;
		ParentGraph->AddNode(Transition, true, true);
		Transition->NodePosX = (EntryA->NodePosX + MoveB->NodePosX) / 2;
		Transition->NodePosY = (EntryA->NodePosY + MoveB->NodePosY) / 2;
		if (EntryA->GetOutputPin() && Transition->GetInputPin())
		{
			EntryA->GetOutputPin()->MakeLinkTo(Transition->GetInputPin());
		}
		if (Transition->GetOutputPin() && MoveB->GetInputPin())
		{
			Transition->GetOutputPin()->MakeLinkTo(MoveB->GetInputPin());
		}
		return true;
	}
	if (Cast<UComboGraphEdNode_Entry>(NodeA) && Cast<UComboGraphEdNode_Transition>(NodeB))
	{
		A->MakeLinkTo(B);
		return true;
	}
	if (MoveA && MoveB)
	{
		if (MoveA->Move == MoveB->Move)
		{
			return false;
		}
		UComboGraphEdNode_Transition* Transition = NewObject<UComboGraphEdNode_Transition>(ParentGraph, UComboGraphEdNode_Transition::StaticClass(), NAME_None, RF_Transactional);
		Transition->From = MoveA->Move;
		Transition->To = MoveB->Move;
		ParentGraph->AddNode(Transition, true, true);
		Transition->NodePosX = (MoveA->NodePosX + MoveB->NodePosX) / 2;
		Transition->NodePosY = (MoveA->NodePosY + MoveB->NodePosY) / 2;
		// Link pins for visual edges
		if (MoveA->GetOutputPin() && Transition->GetInputPin())
		{
			MoveA->GetOutputPin()->MakeLinkTo(Transition->GetInputPin());
		}
		if (Transition->GetOutputPin() && MoveB->GetInputPin())
		{
			Transition->GetOutputPin()->MakeLinkTo(MoveB->GetInputPin());
		}
		return true;
	}
	if (MoveA && TrB)
	{
		TrB->From = MoveA->Move;
		A->MakeLinkTo(B);
		return true;
	}
	if (TrA && MoveB)
	{
		TrA->To = MoveB->Move;
		A->MakeLinkTo(B);
		return true;
	}
	return false;
}
