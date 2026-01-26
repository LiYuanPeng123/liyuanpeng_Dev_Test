#include "CancerStateMachineEditorToolkit.h"
#include "EdGraph/EdGraph.h"
#include "CancerStateMachineData.h"
#include "CancerStateMachineEdGraph.h"
#include "CancerStateMachineSchema.h"
#include "GraphEditor.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Nodes/CancerStateMachineEdNode.h"
#include "Nodes/CancerStateMachineEdNodeEdge.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "EdGraphNode_Comment.h"
#include "ScopedTransaction.h"
#include "EdGraphUtilities.h"
#include "GraphEditAction.h"

// 标签页 ID 常量定义
const FName FCancerStateMachineEditorToolkit::GraphCanvasTabId(TEXT("CancerStateMachineEditor_GraphCanvas"));
const FName FCancerStateMachineEditorToolkit::PropertiesTabId(TEXT("CancerStateMachineEditor_Properties"));

void FCancerStateMachineEditorToolkit::Initialize(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& EditWithinLevelEditor, UCancerStateMachineData* InData)
{
	StateMachineData = InData;

	// 1. 绑定编辑器命令
	BindCommands();
	// 2. 创建图表数据结构
	CreateEdGraph();
	// 3. 创建 UI 控件
	CreateWidgets();

	// 定义编辑器布局
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_CancerStateMachineEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(GraphCanvasTabId, ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(PropertiesTabId, ETabState::OpenedTab)
					->SetSizeCoefficient(0.3f)
				)
			)
		);

	// 初始化资产编辑器
	InitAssetEditor(Mode, EditWithinLevelEditor, "CancerStateMachineEditor", StandaloneDefaultLayout, true, true, InData);
}

FName FCancerStateMachineEditorToolkit::GetToolkitFName() const { return FName("CancerStateMachineEditor"); }
FText FCancerStateMachineEditorToolkit::GetBaseToolkitName() const { return INVTEXT("Cancer State Machine Editor"); }
FString FCancerStateMachineEditorToolkit::GetWorldCentricTabPrefix() const { return TEXT("CancerStateMachineEditor"); }
FLinearColor FCancerStateMachineEditorToolkit::GetWorldCentricTabColorScale() const { return FLinearColor::White; }

void FCancerStateMachineEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	// 注册图表画布标签页生成器
	InTabManager->RegisterTabSpawner(GraphCanvasTabId, FOnSpawnTab::CreateSP(this, &FCancerStateMachineEditorToolkit::SpawnTab_GraphCanvas))
		.SetDisplayName(INVTEXT("Viewport"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	// 注册属性详情标签页生成器
	InTabManager->RegisterTabSpawner(PropertiesTabId, FOnSpawnTab::CreateSP(this, &FCancerStateMachineEditorToolkit::SpawnTab_Properties))
		.SetDisplayName(INVTEXT("Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FCancerStateMachineEditorToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(GraphCanvasTabId);
	InTabManager->UnregisterTabSpawner(PropertiesTabId);
}

void FCancerStateMachineEditorToolkit::SaveAsset_Execute()
{
	// 保存前先将图表数据同步到资产
	if (EdGraph)
	{
		EdGraph->SaveGraph();
	}

	FAssetEditorToolkit::SaveAsset_Execute();
}

void FCancerStateMachineEditorToolkit::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	if (EdGraph && PropertyThatChanged)
	{
		const FName PropertyName = PropertyThatChanged->GetFName();

		// 如果修改的是节点名称
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UCancerStateMachineNode, NodeName))
		{
			// 处理通过细节面板修改名称的情况，确保唯一性
			for (const TWeakObjectPtr<UObject>& WeakObj : DetailsView->GetSelectedObjects())
			{
				if (UCancerStateMachineNode* RuntimeNode = Cast<UCancerStateMachineNode>(WeakObj.Get()))
				{
					// 寻找对应的 EdNode
					for (UEdGraphNode* GraphNode : EdGraph->Nodes)
					{
						if (UCancerStateMachineEdNode* EdNode = Cast<UCancerStateMachineEdNode>(GraphNode))
						{
							if (EdNode->RuntimeNode == RuntimeNode)
							{
								// 触发重命名逻辑（包含唯一性检查）
								EdNode->OnRenameNode(RuntimeNode->NodeName.ToString());
								break;
							}
						}
					}
				}
			}
		}

		// 通知图表刷新 UI
		EdGraph->NotifyGraphChanged();
	}
}

TSharedRef<SDockTab> FCancerStateMachineEditorToolkit::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab).Label(INVTEXT("Viewport"))[GraphEditorWidget.ToSharedRef()];
}

TSharedRef<SDockTab> FCancerStateMachineEditorToolkit::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab).Label(INVTEXT("Details"))[DetailsView.ToSharedRef()];
}

void FCancerStateMachineEditorToolkit::CreateWidgets()
{
	// 配置图表编辑器事件
	SGraphEditor::FGraphEditorEvents Events;
	Events.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FCancerStateMachineEditorToolkit::OnSelectedNodesChanged);
	Events.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FCancerStateMachineEditorToolkit::OnNodeDoubleClicked);

	// 创建图表编辑器控件
	GraphEditorWidget = SNew(SGraphEditor)
		.AdditionalCommands(GetToolkitCommands())
		.GraphToEdit(EdGraph)
		.GraphEvents(Events);

	// 配置并创建属性详情面板
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = true;
	DetailsViewArgs.bUpdatesFromSelection = true;
	DetailsViewArgs.NotifyHook = this;

	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	// 默认显示资产本身的属性
	DetailsView->SetObject(StateMachineData);
}

void FCancerStateMachineEditorToolkit::CreateEdGraph()
{
	bool bIsNewGraph = (StateMachineData->EdGraph == nullptr);

	if (bIsNewGraph)
	{
		// 创建新的图表对象
		StateMachineData->EdGraph = FBlueprintEditorUtils::CreateNewGraph(StateMachineData, NAME_None, UCancerStateMachineEdGraph::StaticClass(), UCancerStateMachineSchema::StaticClass());
	}

	EdGraph = Cast<UCancerStateMachineEdGraph>(StateMachineData->EdGraph);
	if (EdGraph)
	{
		EdGraph->StateMachineData = StateMachineData;
		// 监听图表变化事件
		EdGraph->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateSP(this, &FCancerStateMachineEditorToolkit::OnGraphChanged));
		
		// 重建图表（如果是新资产，会生成默认节点）
		EdGraph->RebuildGraph();
	}
}

void FCancerStateMachineEditorToolkit::OnGraphChanged(const FEdGraphEditAction& Action)
{
	// 图表变化时自动保存数据到资产
	if (EdGraph)
	{
		// 只有在结构发生变化（添加/删除节点）时才需要重构运行时拓扑
		// 注意：连线变化通常不带特定的 GRAPHACTION 标志，但会触发此回调
		bool bShouldRebuild = (Action.Action & (GRAPHACTION_AddNode | GRAPHACTION_RemoveNode));
		
		// 如果是连线变化，Action.Action 可能是 GRAPHACTION_Default (0)
		// 我们通过检查 Nodes 是否为空来判断是否有实质性变化（排除单纯的 UI 刷新）
		if (!bShouldRebuild && Action.Nodes.Num() > 0)
		{
			bShouldRebuild = true;
		}
		
		if (bShouldRebuild)
		{
			EdGraph->SaveGraph();
		}
	}

	// 强制刷新详情面板，确保名称等属性同步
	if (DetailsView.IsValid())
	{
		DetailsView->SetObjects(DetailsView->GetSelectedObjects());
	}
}

void FCancerStateMachineEditorToolkit::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	if (NewSelection.Num() > 0)
	{
		// 选中的对象列表
		TArray<UObject*> SelectedObjects;
		for (UObject* Obj : NewSelection)
		{
			//如果选中的是条件节点和执行任务节点 需要聚焦到RuntimeData的细节面版
			if (UCancerStateMachineEdNode_Task* EdNode = Cast<UCancerStateMachineEdNode_Task>(Obj))
			{
				if (EdNode->RuntimeNode)
				{
					SelectedObjects.Add(EdNode->RuntimeNode);
				}
				else
				{
					SelectedObjects.Add(EdNode);
				}
			}
			else if (UCancerStateMachineEdNodeEdgeBase* EdEdge = Cast<UCancerStateMachineEdNodeEdgeBase>(Obj))
			{
				SelectedObjects.Add(EdEdge);
			}
			else
			{
				SelectedObjects.Add(Obj);
			}
		}
		DetailsView->SetObjects(SelectedObjects);
	}
	else
	{
		// 未选中任何节点时，恢复显示资产本身的属性
		DetailsView->SetObject(StateMachineData);
	}
}

void FCancerStateMachineEditorToolkit::OnNodeDoubleClicked(UEdGraphNode* Node)
{
	if (UCancerStateMachineEdNode* EdNode = Cast<UCancerStateMachineEdNode>(Node))
	{
		EdNode->OnNodeDoubleClicked();
	}
}

void FCancerStateMachineEditorToolkit::BindCommands()
{
	// 绑定复制/剪切/粘贴/重复命令
	GetToolkitCommands()->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::CopySelectedNodes),
		FCanExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::CanCopyNodes));

	GetToolkitCommands()->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::CutSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::CanCutNodes));

	GetToolkitCommands()->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::PasteNodes),
		FCanExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::CanPasteNodes));

	GetToolkitCommands()->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::DuplicateNodes),
		FCanExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::CanDuplicateNodes));

	GetToolkitCommands()->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::OnRenameNode),
		FCanExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::CanRenameNodes));

	// 绑定通用的删除命令
	GetToolkitCommands()->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::DeleteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::CanDeleteNodes));

	// 绑定创建注释命令 (快捷键 C)
	GetToolkitCommands()->MapAction(FGraphEditorCommands::Get().CreateComment,
		FExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::OnCreateComment),
		FCanExecuteAction::CreateSP(this, &FCancerStateMachineEditorToolkit::CanCreateComment));
}

void FCancerStateMachineEditorToolkit::DeleteSelectedNodes()
{
	if (!GraphEditorWidget.IsValid()) return;

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());
	GraphEditorWidget->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = GraphEditorWidget->GetSelectedNodes();
	GraphEditorWidget->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
		{
			if (Node->CanUserDeleteNode())
			{
				FBlueprintEditorUtils::RemoveNode(nullptr, Node, true);
			}
		}
	}
}

bool FCancerStateMachineEditorToolkit::CanDeleteNodes() const
{
	if (!GraphEditorWidget.IsValid()) return false;
	const FGraphPanelSelectionSet SelectedNodes = GraphEditorWidget->GetSelectedNodes();
	return SelectedNodes.Num() > 0;
}

void FCancerStateMachineEditorToolkit::CopySelectedNodes()
{
	FGraphPanelSelectionSet SelectedNodes = GraphEditorWidget->GetSelectedNodes();
	
	// 过滤掉不可复制的节点（如入口节点）
	for (FGraphPanelSelectionSet::TIterator It(SelectedNodes); It; ++It)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*It);
		if (Node && !Node->CanDuplicateNode())
		{
			It.RemoveCurrent();
		}
	}

	if (SelectedNodes.Num() > 0)
	{
		// 将选中的节点导出为文本并存入剪贴板
		FString ExportedText;
		FEdGraphUtilities::ExportNodesToText(SelectedNodes, /*out*/ ExportedText);
		FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
	}
}

bool FCancerStateMachineEditorToolkit::CanCopyNodes() const
{
	if (!GraphEditorWidget.IsValid()) return false;
	const FGraphPanelSelectionSet SelectedNodes = GraphEditorWidget->GetSelectedNodes();
	
	for (UObject* NodeObj : SelectedNodes)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(NodeObj);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}
	
	return false;
}

void FCancerStateMachineEditorToolkit::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedNodes();
}

bool FCancerStateMachineEditorToolkit::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FCancerStateMachineEditorToolkit::PasteNodes()
{
	if (!GraphEditorWidget.IsValid()) return;

	// 从剪贴板获取文本
	FString ClipboardText;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardText);

	const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
	UEdGraph* Graph = GraphEditorWidget->GetCurrentGraph();
	Graph->Modify();

	// 将文本导入为节点
	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(Graph, ClipboardText, /*out*/ PastedNodes);

	// 这里的关键：对于自定义节点，我们需要确保里面的 Runtime 对象也被深拷贝，
	// 否则粘贴出来的节点会和原节点共享同一个 Runtime 对象。
	for (UEdGraphNode* Node : PastedNodes)
	{
		Node->CreateNewGuid(); // 确保 ID 唯一

		if (UCancerStateMachineEdNode* EdNode = Cast<UCancerStateMachineEdNode>(Node))
		{
			if (EdNode->RuntimeNode)
			{
				// 深拷贝运行时节点数据
				EdNode->RuntimeNode = DuplicateObject<UCancerStateMachineNode>(EdNode->RuntimeNode, EdNode);
				EdNode->RuntimeNode->SetFlags(RF_Transactional);

				// 确保粘贴后的名称唯一
				EdNode->OnRenameNode(EdNode->RuntimeNode->NodeName.ToString());
			}
		}
		else if (UCancerStateMachineEdNodeEdge* EdEdge = Cast<UCancerStateMachineEdNodeEdge>(Node))
		{
			if (EdEdge->RuntimeEdge)
			{
				// 深拷贝运行时边数据
				EdEdge->RuntimeEdge = DuplicateObject<UCancerStateMachineEdge>(EdEdge->RuntimeEdge, EdEdge);
				EdEdge->RuntimeEdge->SetFlags(RF_Transactional);
			}
		}
	}

	// 粘贴时尝试使用鼠标当前位置，如果不可用则使用默认位置
	FVector2f PasteLocation = GraphEditorWidget->GetPasteLocation2f();
	EdGraph->StateMachineData->EdGraph = EdGraph; // 确保数据一致性
	
	if (PastedNodes.Num() > 0)
	{
		// 1. 计算粘贴出的节点群组的左上角边界
		float MinX = FLT_MAX;
		float MinY = FLT_MAX;
		for (UEdGraphNode* Node : PastedNodes)
		{
			MinX = FMath::Min(MinX, (float)Node->NodePosX);
			MinY = FMath::Min(MinY, (float)Node->NodePosY);
		}

		// 2. 计算从原左上角到目标鼠标位置的偏移量
		FVector2f Offset = PasteLocation - FVector2f(MinX, MinY);

		// 3. 应用偏移量
		for (UEdGraphNode* Node : PastedNodes)
		{
			Node->NodePosX += (int32)Offset.X;
			Node->NodePosY += (int32)Offset.Y;
		}
	}

	GraphEditorWidget->NotifyGraphChanged();
}

bool FCancerStateMachineEditorToolkit::CanPasteNodes() const
{
	if (!GraphEditorWidget.IsValid()) return false;
	FString ClipboardText;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardText);
	return !ClipboardText.IsEmpty();
}

void FCancerStateMachineEditorToolkit::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FCancerStateMachineEditorToolkit::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FCancerStateMachineEditorToolkit::OnRenameNode()
{
	if (GraphEditorWidget.IsValid())
	{
		const FGraphPanelSelectionSet SelectedNodes = GraphEditorWidget->GetSelectedNodes();
		for (UObject* NodeObj : SelectedNodes)
		{
			UEdGraphNode* Node = Cast<UEdGraphNode>(NodeObj);
			if (Node && Node->bCanRenameNode)
			{
				GraphEditorWidget->IsNodeTitleVisible(Node, true);
				break;
			}
		}
	}
}

bool FCancerStateMachineEditorToolkit::CanRenameNodes() const
{
	if (!GraphEditorWidget.IsValid()) return false;
	const FGraphPanelSelectionSet SelectedNodes = GraphEditorWidget->GetSelectedNodes();
	if (SelectedNodes.Num() == 1)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedNodes.CreateConstIterator());
		return Node && Node->bCanRenameNode;
	}
	return false;
}

void FCancerStateMachineEditorToolkit::OnCreateComment()
{
	if (UEdGraph* Graph = GraphEditorWidget.IsValid() ? GraphEditorWidget->GetCurrentGraph() : nullptr)
	{
		// 获取 Schema 中定义的注释动作并执行
		const TSharedPtr<FEdGraphSchemaAction> Action = Graph->GetSchema()->GetCreateCommentAction();
		if (Action.IsValid())
		{
			TArray<UEdGraphPin*> Pins;
			Action->PerformAction(Graph, Pins, FVector2f());
		}
	}
}

bool FCancerStateMachineEditorToolkit::CanCreateComment() const
{
	// 只有在有选中节点时才允许通过快捷键创建包裹式注释
	return GraphEditorWidget.IsValid() ? (GraphEditorWidget->GetNumberOfSelectedNodes() != 0) : false;
}
