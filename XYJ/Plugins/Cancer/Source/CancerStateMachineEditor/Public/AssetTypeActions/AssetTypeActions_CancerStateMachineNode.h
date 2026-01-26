// Copyright 2026 Cancer State Machine. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "CancerStateMachineNodeBlueprint.h"

/**
 * FAssetTypeActions_CancerStateMachineNode
 * 状态机节点蓝图资产类型操作类。
 */
class FAssetTypeActions_CancerStateMachineNode : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_CancerStateMachineNode(EAssetTypeCategories::Type InCategory) : MyCategory(InCategory) {}

	//~ Begin IAssetTypeActions Interface
	virtual FText GetName() const override { return NSLOCTEXT("CancerStateMachine", "AssetTypeActions_CancerStateMachineNode", "Cancer State Node (Blueprint)"); }
	virtual FColor GetTypeColor() const override { return FColor(128, 0, 128); } // 紫色
	virtual UClass* GetSupportedClass() const override { return UCancerStateMachineNodeBlueprint::StaticClass(); }
	virtual uint32 GetCategories() override { return MyCategory; }
	//~ End IAssetTypeActions Interface

private:
	EAssetTypeCategories::Type MyCategory;
};
