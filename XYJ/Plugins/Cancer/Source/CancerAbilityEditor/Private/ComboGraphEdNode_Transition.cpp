#include "ComboGraphEdNode_Transition.h"
#include "CancerAbility/Public/Combo/MoveDefinition.h"

FText UComboGraphEdNode_Transition::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	FString Title = TEXT("Transition");
	if (From && To)
	{
		Title = FString::Printf(TEXT("%s -> %s"), *From->GetName(), *To->GetName());
	}
	return FText::FromString(Title);
}

void UComboGraphEdNode_Transition::AllocateDefaultPins()
{
	InPin = CreatePin(EGPD_Input, TEXT("Combo"), TEXT("In"));
	OutPin = CreatePin(EGPD_Output, TEXT("Combo"), TEXT("Out"));
}
