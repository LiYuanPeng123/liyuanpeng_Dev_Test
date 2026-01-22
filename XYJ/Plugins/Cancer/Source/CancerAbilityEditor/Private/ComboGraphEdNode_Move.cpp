#include "ComboGraphEdNode_Move.h"
#include "CancerAbility/Public/Combo/MoveDefinition.h"

FText UComboGraphEdNode_Move::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (Move)
	{
		return FText::FromString(Move->GetName());
	}
	return FText::FromString(TEXT("Move"));
}

void UComboGraphEdNode_Move::AllocateDefaultPins()
{
	InputPin = CreatePin(EGPD_Input, TEXT("Combo"), TEXT("In"));
	OutputPin = CreatePin(EGPD_Output, TEXT("Combo"), TEXT("Out"));
}
