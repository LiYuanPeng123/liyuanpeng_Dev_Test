#include "ComboGraphSchemaActions.h"
#include "ComboGraphEdNode_Move.h"
#include "ComboGraphEdNode_Transition.h"
#include "EdGraph/EdGraph.h"

UEdGraphNode* FComboGraphSchemaAction_NewMove::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	if (!ParentGraph) return nullptr;
	UComboGraphEdNode_Move* Node = NewObject<UComboGraphEdNode_Move>(ParentGraph, UComboGraphEdNode_Move::StaticClass(), NAME_None, RF_Transactional);
	ParentGraph->AddNode(Node, bSelectNewNode, bSelectNewNode);
	Node->NodePosX = Location.X;
	Node->NodePosY = Location.Y;
	return Node;
}

UEdGraphNode* FComboGraphSchemaAction_NewTransition::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	if (!ParentGraph) return nullptr;
	UComboGraphEdNode_Transition* Node = NewObject<UComboGraphEdNode_Transition>(ParentGraph, UComboGraphEdNode_Transition::StaticClass(), NAME_None, RF_Transactional);
	ParentGraph->AddNode(Node, bSelectNewNode, bSelectNewNode);
	Node->NodePosX = Location.X;
	Node->NodePosY = Location.Y;
	return Node;
}

