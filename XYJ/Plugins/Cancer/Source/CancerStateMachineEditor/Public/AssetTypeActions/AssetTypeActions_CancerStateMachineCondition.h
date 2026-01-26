#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "CancerStateMachineCondition.h"

/**
 * FAssetTypeActions_CancerStateMachineCondition
 * 状态机条件资产类型操作类
 */
class FAssetTypeActions_CancerStateMachineCondition : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_CancerStateMachineCondition(EAssetTypeCategories::Type InCategory) : MyCategory(InCategory) {}

	virtual FText GetName() const override { return INVTEXT("Cancer State Condition"); }
	virtual FColor GetTypeColor() const override { return FColor(0, 255, 128); }
	virtual UClass* GetSupportedClass() const override { return UCancerStateMachineCondition::StaticClass(); }
	virtual uint32 GetCategories() override { return MyCategory; }

private:
	EAssetTypeCategories::Type MyCategory;
};