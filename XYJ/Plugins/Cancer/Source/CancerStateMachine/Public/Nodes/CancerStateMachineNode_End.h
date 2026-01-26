#pragma once

#include "CoreMinimal.h"
#include "CancerStateMachineNode.h"
#include "CancerStateMachineNode_End.generated.h"

/**
 * UCancerStateMachineNode_End
 * 结束状态节点 (运行时)
 * 当状态机进入此节点时，将自动结束状态机任务
 */
UCLASS()
class CANCERSTATEMACHINE_API UCancerStateMachineNode_End : public UCancerStateMachineNode
{
	GENERATED_BODY()

public:
	UCancerStateMachineNode_End();

	virtual void OnActivated_Implementation(UObject* Context) override;
};
