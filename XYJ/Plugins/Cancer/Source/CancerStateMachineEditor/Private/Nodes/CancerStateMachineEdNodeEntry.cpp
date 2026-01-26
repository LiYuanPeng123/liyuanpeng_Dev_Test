
#include "Nodes/CancerStateMachineEdNodeEntry.h"
#include "Settings/CancerStateMachineEditorSettings.h"
#include "CancerStateMachineSchema.h"
#include "EdGraph/EdGraphPin.h"

#define LOCTEXT_NAMESPACE "CancerStateMachineEdNodeEntry"

UCancerStateMachineEdNodeEntry::UCancerStateMachineEdNodeEntry()
{
	bCanRenameNode = 0;
}

void UCancerStateMachineEdNodeEntry::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, UCancerStateMachineSchema::PC_Edge, TEXT("Out"));
}

FText UCancerStateMachineEdNodeEntry::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("EntryNodeTitle", "入口");
}

FLinearColor UCancerStateMachineEdNodeEntry::GetNodeTitleColor() const
{
	return UCancerStateMachineEditorSettings::Get()->EntryNodeColor;
}

#undef LOCTEXT_NAMESPACE
