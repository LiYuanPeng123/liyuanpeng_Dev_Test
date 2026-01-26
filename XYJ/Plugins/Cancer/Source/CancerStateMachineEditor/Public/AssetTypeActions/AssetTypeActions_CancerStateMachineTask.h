#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "CancerStateMachineTask.h"

/**
 * FAssetTypeActions_CancerStateMachineTask
 * 状态机任务资产类型操作类
 */
class FAssetTypeActions_CancerStateMachineTask : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_CancerStateMachineTask(EAssetTypeCategories::Type InCategory) : MyCategory(InCategory) {}

	virtual FText GetName() const override { return INVTEXT("Cancer State Task"); }
	virtual FColor GetTypeColor() const override { return FColor(255, 0, 128); }
	virtual UClass* GetSupportedClass() const override { return UCancerStateMachineTask::StaticClass(); }
	virtual uint32 GetCategories() override { return MyCategory; }

private:
	EAssetTypeCategories::Type MyCategory;
};