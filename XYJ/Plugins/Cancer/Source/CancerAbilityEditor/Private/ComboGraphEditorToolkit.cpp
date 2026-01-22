#include "ComboGraphEditorToolkit.h"
#include "CancerAbility/Public/Combo/ComboGraph.h"
#include "ComboGraphEdGraph.h"
#include "ComboGraphEdNode_Entry.h"
#include "ComboGraphSchema.h"
#include "GraphEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Widgets/Layout/SSplitter.h"

void FComboGraphEditorToolkit::InitEditor(UComboGraph* InGraphAsset)
{
	GraphAsset = InGraphAsset;
	EdGraph = NewObject<UComboGraphEdGraph>(GraphAsset, UComboGraphEdGraph::StaticClass(), NAME_None, RF_Transactional);
	EdGraph->Schema = UComboGraphSchema::StaticClass();
	LoadAssetIntoEdGraph();
	CreateGraphEditor();
}

FName FComboGraphEditorToolkit::GetToolkitFName() const
{
	return FName("ComboGraphEditor");
}

FText FComboGraphEditorToolkit::GetBaseToolkitName() const
{
	return FText::FromString(TEXT("Combo Graph Editor"));
}

FString FComboGraphEditorToolkit::GetWorldCentricTabPrefix() const
{
	return TEXT("ComboGraph");
}

FLinearColor FComboGraphEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.5f, 1.f);
}

void FComboGraphEditorToolkit::CreateGraphEditor()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = FText::FromString(TEXT("Combo Graph"));
	SGraphEditor::FGraphEditorEvents GraphEvents;
	GraphEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateRaw(this, &FComboGraphEditorToolkit::OnGraphSelectionChanged);

	TSharedRef<SGraphEditor> GraphEditor = SNew(SGraphEditor)
		.AdditionalCommands(nullptr)
		.Appearance(AppearanceInfo)
		.GraphToEdit(EdGraph)
		.GraphEvents(GraphEvents);
	GraphEditorWidget = GraphEditor;

	FPropertyEditorModule& PropModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.bAllowSearch = true;
	Args.NotifyHook = nullptr;
	TSharedRef<IDetailsView> DetailsView = PropModule.CreateDetailView(Args);
	DetailsWidget = DetailsView;

	TSharedRef<SDockTab> GraphTab = SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		.Label(FText::FromString(TEXT("Combo Graph Editor")))
		[
			SNew(SSplitter)
			+ SSplitter::Slot()
			.Value(0.7f)
			[
				GraphEditor
			]
			+ SSplitter::Slot()
			.Value(0.3f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Build & Save")))
					.OnClicked_Lambda([this]()
					{
						BuildAssetFromEdGraph();
						if (GraphAsset)
						{
							GraphAsset->MarkPackageDirty();
						}
						return FReply::Handled();
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Create Moves From Selection")))
					.OnClicked_Lambda([this]()
					{
						CreateMoveNodesFromContentBrowserSelection();
						return FReply::Handled();
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Connect Selected Moves")))
					.OnClicked_Lambda([this]()
					{
						ConnectSelectedMoves();
						return FReply::Handled();
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Validate")))
					.OnClicked_Lambda([this]()
					{
						ValidateGraph();
						return FReply::Handled();
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Auto Layout")))
					.OnClicked_Lambda([this]()
					{
						AutoLayout();
						return FReply::Handled();
					})
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.f)
				[
					DetailsView
				]
			]
		];
	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("CancerAbility_ComboGraph_Layout_v2")
		->AddArea(FTabManager::NewPrimaryArea()->Split(FTabManager::NewStack()->AddTab(GetToolkitFName(), ETabState::OpenedTab)));
	InitAssetEditor(EToolkitMode::Standalone, TSharedPtr<IToolkitHost>(), GetToolkitFName(), Layout, true, true, GraphAsset);
	// Replace the default tab content by our built one
	GetTabManager()->InsertNewDocumentTab(GetToolkitFName(), FTabManager::ESearchPreference::PreferLiveTab, GraphTab);
}

void FComboGraphEditorToolkit::OnGraphSelectionChanged(const TSet<UObject*>& NewSelection)
{
	if (DetailsWidget.IsValid())
	{
		TArray<UObject*> Selected;
		for (UObject* Obj : NewSelection) { Selected.Add(Obj); }
		DetailsWidget->SetObjects(Selected);
	}
}

#include "CancerAbility/Public/Combo/ComboGraph.h"
#include "CancerAbility/Public/Combo/MoveDefinition.h"
#include "CancerAbility/Public/Combo/Transition.h"
#include "ComboGraphEdNode_Move.h"
#include "ComboGraphEdNode_Transition.h"
#include "Misc/MessageDialog.h"
#include "EdGraph/EdGraph.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

void FComboGraphEditorToolkit::LoadAssetIntoEdGraph()
{
	if (!GraphAsset || !EdGraph) return;
	UComboGraphEdNode_Entry* Entry = NewObject<UComboGraphEdNode_Entry>(EdGraph, UComboGraphEdNode_Entry::StaticClass(), NAME_None, RF_Transactional);
	EdGraph->AddNode(Entry, false, false);
	Entry->NodePosX = 50;
	Entry->NodePosY = 50;
	TMap<UMoveDefinition*, UComboGraphEdNode_Move*> MoveNodeMap;
	auto EnsureMoveNode = [&](UMoveDefinition* Move)->UComboGraphEdNode_Move*
	{
		if (MoveNodeMap.Contains(Move)) return MoveNodeMap[Move];
		UComboGraphEdNode_Move* Node = NewObject<UComboGraphEdNode_Move>(EdGraph, UComboGraphEdNode_Move::StaticClass(), NAME_None, RF_Transactional);
		Node->Move = Move;
		EdGraph->AddNode(Node, false, false);
		int32 Index = MoveNodeMap.Num();
		Node->NodePosX = 100 + (Index % 5) * 250;
		Node->NodePosY = 100 + (Index / 5) * 200;
		MoveNodeMap.Add(Move, Node);
		return Node;
	};
	for (const FTransition& ST : GraphAsset->StartTransitions.Items)
	{
		UMoveDefinition* To = ST.TargetMove;
		UComboGraphEdNode_Move* ToNode = EnsureMoveNode(To);
		UComboGraphEdNode_Transition* Edge = NewObject<UComboGraphEdNode_Transition>(EdGraph, UComboGraphEdNode_Transition::StaticClass(), NAME_None, RF_Transactional);
		Edge->From = nullptr;
		Edge->To = To;
		Edge->InputTags = ST.InputTags;
		Edge->RequiredStateTags = ST.RequiredStateTags;
		Edge->Priority = ST.Priority;
		EdGraph->AddNode(Edge, false, false);
		Edge->NodePosX = (Entry->NodePosX + ToNode->NodePosX) / 2;
		Edge->NodePosY = (Entry->NodePosY + ToNode->NodePosY) / 2;
		if (Entry->GetOutputPin() && Edge->GetInputPin())
		{
			Entry->GetOutputPin()->MakeLinkTo(Edge->GetInputPin());
		}
		if (Edge->GetOutputPin() && ToNode->GetInputPin())
		{
			Edge->GetOutputPin()->MakeLinkTo(ToNode->GetInputPin());
		}
	}
	for (const auto& Pair : GraphAsset->Transitions)
	{
		UMoveDefinition* From = Pair.Key.Get();
		const FCancerTransitionList& List = Pair.Value;
		UComboGraphEdNode_Move* FromNode = EnsureMoveNode(From);
		for (const FTransition& T : List.Items)
		{
			UMoveDefinition* To = T.TargetMove;
			UComboGraphEdNode_Move* ToNode = EnsureMoveNode(To);
			UComboGraphEdNode_Transition* Edge = NewObject<UComboGraphEdNode_Transition>(EdGraph, UComboGraphEdNode_Transition::StaticClass(), NAME_None, RF_Transactional);
			Edge->From = From;
			Edge->To = To;
			Edge->InputTags = T.InputTags;
			Edge->RequiredStateTags = T.RequiredStateTags;
			Edge->Priority = T.Priority;
			EdGraph->AddNode(Edge, false, false);
			Edge->NodePosX = (FromNode->NodePosX + ToNode->NodePosX) / 2;
			Edge->NodePosY = (FromNode->NodePosY + ToNode->NodePosY) / 2;
		}
	}
}

void FComboGraphEditorToolkit::BuildAssetFromEdGraph()
{
	if (!GraphAsset || !EdGraph) return;
	TMap<TObjectPtr<UMoveDefinition>, FCancerTransitionList> NewTransitions;
	FCancerTransitionList NewStartTransitions;
	UComboGraphEdNode_Entry* EntryNode = nullptr;
	UEdGraphPin* EntryOutPin = nullptr;
	for (UEdGraphNode* Node : EdGraph->Nodes)
	{
		if (UComboGraphEdNode_Entry* EN = Cast<UComboGraphEdNode_Entry>(Node))
		{
			EntryNode = EN;
			EntryOutPin = EN->GetOutputPin();
		}
		if (UComboGraphEdNode_Move* MoveNode = Cast<UComboGraphEdNode_Move>(Node))
		{
		}
		else if (UComboGraphEdNode_Transition* TrNode = Cast<UComboGraphEdNode_Transition>(Node))
		{
			if (TrNode->From && TrNode->To)
			{
				FCancerTransitionList& List = NewTransitions.FindOrAdd(TrNode->From);
				FTransition T;
				T.TargetMove = TrNode->To;
				T.InputTags = TrNode->InputTags;
				T.RequiredStateTags = TrNode->RequiredStateTags;
				T.Priority = TrNode->Priority;
				List.Items.Add(T);
			}
			else if (!TrNode->From && TrNode->To)
			{
				bool bLinkedFromEntry = false;
				if (EntryOutPin && TrNode->GetInputPin())
				{
					for (UEdGraphPin* Linked : EntryOutPin->LinkedTo)
					{
						if (Linked == TrNode->GetInputPin())
						{
							bLinkedFromEntry = true;
							break;
						}
					}
				}
				if (bLinkedFromEntry)
				{
					FTransition T;
					T.TargetMove = TrNode->To;
					T.InputTags = TrNode->InputTags;
					T.RequiredStateTags = TrNode->RequiredStateTags;
					T.Priority = TrNode->Priority;
					NewStartTransitions.Items.Add(T);
				}
			}
		}
	}
	GraphAsset->Transitions = NewTransitions;
	GraphAsset->StartTransitions = NewStartTransitions;
}

void FComboGraphEditorToolkit::ConnectSelectedMoves()
{
	if (!GraphEditorWidget || !EdGraph) return;
	FGraphPanelSelectionSet Selection = GraphEditorWidget->GetSelectedNodes();
	TArray<UComboGraphEdNode_Move*> MoveNodes;
	for (UObject* Obj : Selection)
	{
		if (UComboGraphEdNode_Move* MoveNode = Cast<UComboGraphEdNode_Move>(Obj))
		{
			MoveNodes.Add(MoveNode);
		}
	}
	if (MoveNodes.Num() == 2 && MoveNodes[0]->Move && MoveNodes[1]->Move)
	{
		UComboGraphEdNode_Transition* Tr = NewObject<UComboGraphEdNode_Transition>(EdGraph, UComboGraphEdNode_Transition::StaticClass(), NAME_None, RF_Transactional);
		Tr->From = MoveNodes[0]->Move;
		Tr->To = MoveNodes[1]->Move;
		EdGraph->AddNode(Tr, true, true);
		Tr->NodePosX = (MoveNodes[0]->NodePosX + MoveNodes[1]->NodePosX) / 2;
		Tr->NodePosY = (MoveNodes[0]->NodePosY + MoveNodes[1]->NodePosY) / 2;
	}
}

void FComboGraphEditorToolkit::ValidateGraph()
{
	if (!EdGraph) return;
	TArray<FString> Errors;
	bool bHasStartPath = false;
	int32 EntryCount = 0;
	for (UEdGraphNode* Node : EdGraph->Nodes)
	{
		if (UComboGraphEdNode_Move* MoveNode = Cast<UComboGraphEdNode_Move>(Node))
		{
			if (!MoveNode->Move)
			{
				Errors.Add(TEXT("Move 节点未绑定 MoveDefinition"));
			}
			if (MoveNode->GetInputPin() && MoveNode->GetOutputPin())
			{
				const bool bIsolated = MoveNode->GetInputPin()->LinkedTo.Num() == 0 && MoveNode->GetOutputPin()->LinkedTo.Num() == 0;
				if (bIsolated)
				{
					Errors.Add(FString::Printf(TEXT("Move 节点孤立: %s"), *MoveNode->GetName()));
				}
			}
		}
		else if (UComboGraphEdNode_Transition* TrNode = Cast<UComboGraphEdNode_Transition>(Node))
		{
			if (!TrNode->From || !TrNode->To)
			{
				Errors.Add(TEXT("Transition 缺少 From/To"));
			}
			if (TrNode->InputTags.IsEmpty())
			{
				Errors.Add(TEXT("Transition 未配置 InputTags"));
			}
			if (TrNode->From && TrNode->To && TrNode->From == TrNode->To)
			{
				Errors.Add(TEXT("Transition 自环（From 与 To 相同）"));
			}
			if (TrNode->GetInputPin() && TrNode->GetOutputPin())
			{
				if (TrNode->GetInputPin()->LinkedTo.Num() == 0 || TrNode->GetOutputPin()->LinkedTo.Num() == 0)
				{
					Errors.Add(TEXT("Transition 未完整连接（缺少输入或输出连线）"));
				}
			}
			if (!TrNode->From && TrNode->To)
			{
				bHasStartPath = true;
			}
		}
		else if (UComboGraphEdNode_Entry* EN = Cast<UComboGraphEdNode_Entry>(Node))
		{
			EntryCount++;
			UEdGraphPin* Out = EN->GetOutputPin();
			if (Out && Out->LinkedTo.Num() == 0)
			{
				Errors.Add(TEXT("入口节点未连接到任何 Transition"));
			}
		}
	}
	if (EntryCount == 0)
	{
		Errors.Add(TEXT("缺少入口节点"));
	}
	else if (EntryCount > 1)
	{
		Errors.Add(TEXT("存在多个入口节点，仅允许一个"));
	}
	if (!bHasStartPath)
	{
		Errors.Add(TEXT("未设置入口到 Transition 的起始路径"));
	}
	if (Errors.Num() == 0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("校验通过")));
	}
	else
	{
		FString Combined;
		for (const FString& E : Errors)
		{
			Combined += E + TEXT("\n");
		}
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Combined));
	}
}

void FComboGraphEditorToolkit::AutoLayout()
{
	if (!EdGraph) return;
	// 简单层级布局：以 Start 为第 0 层，Transition 指向的 To 为下一层
	TArray<UComboGraphEdNode_Move*> MoveNodes;
	TArray<UComboGraphEdNode_Transition*> TrNodes;
	UComboGraphEdNode_Move* StartNode = nullptr;
	for (UEdGraphNode* Node : EdGraph->Nodes)
	{
		if (UComboGraphEdNode_Move* M = Cast<UComboGraphEdNode_Move>(Node))
		{
			MoveNodes.Add(M);
		}
		else if (UComboGraphEdNode_Transition* T = Cast<UComboGraphEdNode_Transition>(Node))
		{
			TrNodes.Add(T);
		}
	}
	if (!StartNode) return;
	TMap<UMoveDefinition*, int32> Layer;
	Layer.Add(StartNode->Move, 0);
	bool bProgress = true;
	while (bProgress)
	{
		bProgress = false;
		for (UComboGraphEdNode_Transition* T : TrNodes)
		{
			int32* FromLayer = Layer.Find(T->From);
			if (FromLayer && !Layer.Contains(T->To))
			{
				Layer.Add(T->To, *FromLayer + 1);
				bProgress = true;
			}
		}
	}
	// 布局坐标
	TMap<int32, int32> ColumnCount;
	for (UComboGraphEdNode_Move* M : MoveNodes)
	{
		int32 L = 0;
		if (M->Move && Layer.Contains(M->Move)) L = Layer[M->Move];
		int32& Col = ColumnCount.FindOrAdd(L);
		M->NodePosX = 100 + L * 300;
		M->NodePosY = 100 + Col * 200;
		Col++;
	}
	// 过渡节点居中
	for (UComboGraphEdNode_Transition* T : TrNodes)
	{
		UComboGraphEdNode_Move* FromNode = nullptr;
		UComboGraphEdNode_Move* ToNode = nullptr;
		for (UComboGraphEdNode_Move* M : MoveNodes)
		{
			if (M->Move == T->From) FromNode = M;
			if (M->Move == T->To) ToNode = M;
		}
		if (FromNode && ToNode)
		{
			T->NodePosX = (FromNode->NodePosX + ToNode->NodePosX) / 2;
			T->NodePosY = (FromNode->NodePosY + ToNode->NodePosY) / 2;
		}
	}
}

void FComboGraphEditorToolkit::CreateMoveNodesFromContentBrowserSelection()
{
	if (!EdGraph) return;
	FContentBrowserModule& CBModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FAssetData> Selected;
	CBModule.Get().GetSelectedAssets(Selected);
	int32 Index = EdGraph->Nodes.Num();
	for (const FAssetData& Asset : Selected)
	{
		if (Asset.AssetClassPath == UMoveDefinition::StaticClass()->GetClassPathName())
		{
			UMoveDefinition* Move = Cast<UMoveDefinition>(Asset.GetAsset());
			if (!Move) continue;
			UComboGraphEdNode_Move* Node = NewObject<UComboGraphEdNode_Move>(EdGraph, UComboGraphEdNode_Move::StaticClass(), NAME_None, RF_Transactional);
			Node->Move = Move;
			EdGraph->AddNode(Node, true, true);
			Node->NodePosX = 100 + (Index % 5) * 250;
			Node->NodePosY = 100 + (Index / 5) * 200;
			Index++;
		}
	}
}

