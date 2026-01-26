#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "CancerStateMachineData.h"
#include "CancerStateMachineEditorToolkit.h"

/**
 * FAssetTypeActions_CancerStateMachine
 * 状态机资产类型操作类
 * 定义资产在编辑器中的外观和双击打开行为
 */
class FAssetTypeActions_CancerStateMachine : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_CancerStateMachine(EAssetTypeCategories::Type InCategory) : MyCategory(InCategory) {}

	virtual FText GetName() const override { return INVTEXT("Cancer State Machine"); }
	virtual FColor GetTypeColor() const override { return FColor(0, 128, 255); }
	virtual UClass* GetSupportedClass() const override { return UCancerStateMachineData::StaticClass(); }
	virtual uint32 GetCategories() override { return MyCategory; }

	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override
	{
		for (UObject* Obj : InObjects)
		{
			if (UCancerStateMachineData* Data = Cast<UCancerStateMachineData>(Obj))
			{
				TSharedRef<FCancerStateMachineEditorToolkit> NewEditor(new FCancerStateMachineEditorToolkit());
				NewEditor->Initialize(EToolkitMode::Standalone, EditWithinLevelEditor, Data);
			}
		}
	}

private:
	EAssetTypeCategories::Type MyCategory;
};
