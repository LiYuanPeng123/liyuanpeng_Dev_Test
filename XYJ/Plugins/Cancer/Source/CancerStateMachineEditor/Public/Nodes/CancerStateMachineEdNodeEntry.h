#pragma once

#include "CoreMinimal.h"
#include "CancerStateMachineEdNodeBase.h"
#include "CancerStateMachineEdNodeEntry.generated.h"

/**
 * UCancerStateMachineEdNodeEntry
 * 编辑器中的状态机入口节点
 * 标识状态机的起始状态
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNodeEntry : public UCancerStateMachineEdNodeBase
{
	GENERATED_BODY()

public:
	UCancerStateMachineEdNodeEntry();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool CanUserDeleteNode() const override { return false; }
	virtual bool CanDuplicateNode() const override { return false; }
	//~ End UEdGraphNode Interface
};
