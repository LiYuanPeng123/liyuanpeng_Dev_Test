#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"

class UComboGraphEdNode_Move;
class UComboGraphEdNode_Transition;

struct FComboGraphSchemaAction_NewMove final : public FEdGraphSchemaAction
{
	FComboGraphSchemaAction_NewMove()
		: FEdGraphSchemaAction(FText::FromString("Add Move"), FText::FromString("Add Move Node"), FText::GetEmpty(), 0)
	{}

	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};

struct FComboGraphSchemaAction_NewTransition final : public FEdGraphSchemaAction
{
	FComboGraphSchemaAction_NewTransition()
		: FEdGraphSchemaAction(FText::FromString("Add Transition"), FText::FromString("Add Transition Node"), FText::GetEmpty(), 0)
	{}

	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};

