#include "ComboGraphEdNode_Entry.h"

FText UComboGraphEdNode_Entry::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Entry"));
}

void UComboGraphEdNode_Entry::AllocateDefaultPins()
{
	OutPin = CreatePin(EGPD_Output, TEXT("Combo"), TEXT("Out"));
}

