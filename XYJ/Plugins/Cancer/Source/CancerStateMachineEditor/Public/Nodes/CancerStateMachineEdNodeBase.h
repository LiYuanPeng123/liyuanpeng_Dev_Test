#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "CancerStateMachineEdNodeBase.generated.h"

/**
 * UCancerStateMachineEdNodeBase
 * 状态机编辑器节点基类
 * 提供通用的引脚操作和基础功能
 */
UCLASS(Abstract)
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNodeBase : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UCancerStateMachineEdNodeBase();
	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode Interface

	/** 获取输入引脚 */
	virtual UEdGraphPin* GetInputPin() const;
	/** 获取输出引脚 (默认 Transitions) */
	virtual UEdGraphPin* GetOutputPin() const;
	/** 获取完成引脚 (OnFinished) */
	virtual UEdGraphPin* GetFinishPin() const;

	/** 获取节点背景颜色 */
	virtual FLinearColor GetBackgroundColor() const { return FLinearColor::Black; }
};
