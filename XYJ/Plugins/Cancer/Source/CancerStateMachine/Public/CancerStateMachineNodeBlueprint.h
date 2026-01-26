// Copyright 2026 Cancer State Machine. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h"
#include "CancerStateMachineNodeBlueprint.generated.h"

/**
 * UCancerStateMachineNodeBlueprint
 * 专门用于 Cancer 状态机节点的蓝图类资产。
 * 类似于 ComboGraphNodeBlueprint，它在内容浏览器中表现为一个独立的资产类型。
 */
UCLASS(BlueprintType)
class CANCERSTATEMACHINE_API UCancerStateMachineNodeBlueprint : public UBlueprint
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	//~ Begin UBlueprint Interface
	virtual bool SupportedByDefaultBlueprintFactory() const override { return false; }
	//~ End UBlueprint Interface
#endif
};
