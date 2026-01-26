#pragma once

#include "CoreMinimal.h"
#include "Nodes/CancerStateMachineEdNode.h"
#include "CancerStateMachineEdNode_End.generated.h"

/**
 * UCancerStateMachineEdNode_End
 * 结束状态节点 (编辑器)
 */
UCLASS(HideCategories = ("Settings", "State Config"))
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNode_End : public UCancerStateMachineEdNode
{
	GENERATED_BODY()

public:
	//~ Begin UEdGraphNode Interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool GetCanRenameNode() const override { return false; }
	virtual void AllocateDefaultPins() override;
	virtual void OnNodeDoubleClicked() override;
	//~ End UEdGraphNode Interface
};
