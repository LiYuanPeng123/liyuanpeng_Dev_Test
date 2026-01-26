#include "Nodes/CancerStateMachineEdNode_End.h"
#include "Settings/CancerStateMachineEditorSettings.h"
#include "CancerStateMachineSchema.h"
#include "Nodes/CancerStateMachineNode_End.h"

#define LOCTEXT_NAMESPACE "CancerStateMachineEdNode_End"

FText UCancerStateMachineEdNode_End::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("EndNodeTitle", "End");
}

FLinearColor UCancerStateMachineEdNode_End::GetNodeTitleColor() const
{
	return UCancerStateMachineEditorSettings::Get()->EndNodeColor;
}

void UCancerStateMachineEdNode_End::AllocateDefaultPins()
{
	// 结束节点只有输入，没有输出
	CreatePin(EGPD_Input, UCancerStateMachineSchema::PC_Edge, TEXT("In"));
}

void UCancerStateMachineEdNode_End::OnNodeDoubleClicked()
{
	// 结束节点是固定逻辑，双击不执行任何操作（不打开蓝图）
}

#undef LOCTEXT_NAMESPACE
