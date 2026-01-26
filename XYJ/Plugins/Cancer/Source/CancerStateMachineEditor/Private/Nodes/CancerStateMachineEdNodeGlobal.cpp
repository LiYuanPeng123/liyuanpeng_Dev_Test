#include "Nodes/CancerStateMachineEdNodeGlobal.h"
#include "Settings/CancerStateMachineEditorSettings.h"
#include "CancerStateMachineSchema.h"

#define LOCTEXT_NAMESPACE "CancerStateMachineEdNodeGlobal"

UCancerStateMachineEdNodeGlobal::UCancerStateMachineEdNodeGlobal()
{
	bCanRenameNode = 0;
}

void UCancerStateMachineEdNodeGlobal::AllocateDefaultPins()
{
	// 全局节点只需要输出引脚，用于连接到目标状态或条件节点
	CreatePin(EGPD_Output, UCancerStateMachineSchema::PC_Edge, TEXT("Out"));
}

FText UCancerStateMachineEdNodeGlobal::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GlobalNodeTitle", "全局 (Global)");
}

FLinearColor UCancerStateMachineEdNodeGlobal::GetNodeTitleColor() const
{
	return UCancerStateMachineEditorSettings::Get()->GlobalNodeColor;
}

#undef LOCTEXT_NAMESPACE
