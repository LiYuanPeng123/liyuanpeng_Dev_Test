#pragma once

#include "CoreMinimal.h"
#include "CancerStateMachineEdNodeBase.h"
#include "CancerStateMachineEdNodeGlobal.generated.h"

/**
 * UCancerStateMachineEdNodeGlobal
 * 编辑器中的全局节点
 * 用于定义全局转换（Global Transitions）
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNodeGlobal : public UCancerStateMachineEdNodeBase
{
	GENERATED_BODY()

public:
	UCancerStateMachineEdNodeGlobal();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool CanUserDeleteNode() const override { return true; }
	//~ End UEdGraphNode Interface
};
