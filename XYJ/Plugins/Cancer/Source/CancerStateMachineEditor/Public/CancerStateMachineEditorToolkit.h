#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"

class UCancerStateMachineData;
class UCancerStateMachineEdGraph;

/**
 * FCancerStateMachineEditorToolkit
 * 状态机编辑器工具箱类
 * 管理编辑器窗口、标签页布局、图表交互以及资产数据的编辑
 */
class CANCERSTATEMACHINEEDITOR_API FCancerStateMachineEditorToolkit : public FAssetEditorToolkit, public FNotifyHook
{
public:
	/** 初始化编辑器工具箱 */
	void Initialize(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& EditWithinLevelEditor, UCancerStateMachineData* InData);

	//~ Begin IToolkit 接口
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	//~ End IToolkit 接口

	//~ Begin FAssetEditorToolkit 接口
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void SaveAsset_Execute() override;
	//~ End FAssetEditorToolkit 接口

	//~ Begin FNotifyHook 接口
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;
	//~ End FNotifyHook 接口

private:
	/** 生成图表画布标签页 */
	TSharedRef<SDockTab> SpawnTab_GraphCanvas(const FSpawnTabArgs& Args);
	/** 生成属性详情标签页 */
	TSharedRef<SDockTab> SpawnTab_Properties(const FSpawnTabArgs& Args);

	/** 创建编辑器所需的控件 */
	void CreateWidgets();
	/** 创建编辑器的 EdGraph 实例 */
	void CreateEdGraph();
	/** 绑定编辑器命令（如删除、复制、粘贴、注释等） */
	void BindCommands();

	// 命令处理函数
	/** 复制选中的节点 */
	void CopySelectedNodes();
	/** 判断是否可以复制节点 */
	bool CanCopyNodes() const;
	/** 剪切选中的节点 */
	void CutSelectedNodes();
	/** 判断是否可以剪切节点 */
	bool CanCutNodes() const;
	/** 粘贴节点 */
	void PasteNodes();
	/** 判断是否可以粘贴节点 */
	bool CanPasteNodes() const;
	/** 复制并粘贴选中节点（Duplicate） */
	void DuplicateNodes();
	/** 判断是否可以执行 Duplicate */
	bool CanDuplicateNodes() const;
	/** 开始重命名选中的节点 */
	void OnRenameNode();
	/** 判断是否可以重命名选中的节点 */
	bool CanRenameNodes() const;

	/** 删除选中的节点 */
	void DeleteSelectedNodes();
	/** 判断是否可以删除节点 */
	bool CanDeleteNodes() const;
	/** 创建注释节点的回调 */
	void OnCreateComment();
	/** 判断是否可以创建注释 */
	bool CanCreateComment() const;

	/** 当图表内容发生变化时的回调 */
	void OnGraphChanged(const FEdGraphEditAction& Action);
	/** 当选中的节点发生变化时的回调 */
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);
	/** 当节点被双击时的回调 */
	void OnNodeDoubleClicked(UEdGraphNode* Node);

	/** 正在编辑的资产数据 */
	UCancerStateMachineData* StateMachineData;
	/** 关联的编辑器图表 */
	UCancerStateMachineEdGraph* EdGraph;

	/** 图表编辑器控件实例 */
	TSharedPtr<class SGraphEditor> GraphEditorWidget;
	/** 属性详情面板控件实例 */
	TSharedPtr<class IDetailsView> DetailsView;

	// 标签页 ID 定义
	static const FName GraphCanvasTabId;
	static const FName PropertiesTabId;
};
