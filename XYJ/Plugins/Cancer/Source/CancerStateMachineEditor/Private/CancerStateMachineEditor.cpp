#include "CancerStateMachineEditor.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions/AssetTypeActions_CancerStateMachine.h"
#include "AssetTypeActions/AssetTypeActions_CancerStateMachineCondition.h"
#include "AssetTypeActions/AssetTypeActions_CancerStateMachineTask.h"
#include "AssetTypeActions/AssetTypeActions_CancerStateMachineNode.h"
#include "EdGraphUtilities.h"

#define LOCTEXT_NAMESPACE "FCancerStateMachineEditorModule"

void FCancerStateMachineEditorModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	
	// 注册自定义资产分类：Cancer
	CancerAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Cancer")), LOCTEXT("CancerCategory", "Cancer"));

	// 注册状态机数据资产操作
	TSharedPtr<FAssetTypeActions_CancerStateMachine> DataAction = MakeShareable(new FAssetTypeActions_CancerStateMachine(CancerAssetCategory));
	AssetTools.RegisterAssetTypeActions(DataAction.ToSharedRef());
	RegisteredAssetTypeActions.Add(DataAction);

	// 注册状态机任务资产操作
	TSharedPtr<FAssetTypeActions_CancerStateMachineTask> TaskAction = MakeShareable(new FAssetTypeActions_CancerStateMachineTask(CancerAssetCategory));
	AssetTools.RegisterAssetTypeActions(TaskAction.ToSharedRef());
	RegisteredAssetTypeActions.Add(TaskAction);

	// 注册状态机条件资产操作
	TSharedPtr<FAssetTypeActions_CancerStateMachineCondition> ConditionAction = MakeShareable(new FAssetTypeActions_CancerStateMachineCondition(CancerAssetCategory));
	AssetTools.RegisterAssetTypeActions(ConditionAction.ToSharedRef());
	RegisteredAssetTypeActions.Add(ConditionAction);

	// 注册状态机节点蓝图资产操作
	TSharedPtr<FAssetTypeActions_CancerStateMachineNode> NodeAction = MakeShareable(new FAssetTypeActions_CancerStateMachineNode(CancerAssetCategory));
	AssetTools.RegisterAssetTypeActions(NodeAction.ToSharedRef());
	RegisteredAssetTypeActions.Add(NodeAction);
}

void FCancerStateMachineEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (auto Action : RegisteredAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
		}
	}
	RegisteredAssetTypeActions.Empty();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FCancerStateMachineEditorModule, CancerStateMachineEditor)