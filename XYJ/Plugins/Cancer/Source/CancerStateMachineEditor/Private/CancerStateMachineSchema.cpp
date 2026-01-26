#include "CancerStateMachineSchema.h"
#include "CancerStateMachineSchemaHelpers.h"

#include "EdGraphNode_Comment.h"
#include "ScopedTransaction.h"
#include "EdGraph/EdGraph.h"
#include "Nodes/CancerStateMachineEdNode.h"
#include "Nodes/CancerStateMachineEdNodeEdge.h"
#include "Nodes/CancerStateMachineEdNodeEntry.h"
#include "Nodes/CancerStateMachineEdNodeGlobal.h"
#include "Nodes/CancerStateMachineEdNode_End.h"
#include "Nodes/CancerStateMachineNode_SubStateMachine.h"
#include "CancerStateMachineEdge.h"
#include "GraphEditor.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "GraphEditorActions.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "CancerStateMachineNodeBlueprint.h"
#include "Factorys/CancerStateMachineNodeFactory.h"
#include "Framework/Commands/GenericCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Nodes/CancerStateMachineNode_End.h"

#define LOCTEXT_NAMESPACE "CancerStateMachineSchema"

const FName UCancerStateMachineSchema::PC_Edge(TEXT("CancerStateMachine"));

const FLinearColor UCancerStateMachineSchema::Color_StateNode(0.5f, 0.5f, 0.5f); // 默认灰色
const FLinearColor UCancerStateMachineSchema::Color_GlobalNode(0.5f, 0.2f, 0.8f); // 紫色/品红
const FLinearColor UCancerStateMachineSchema::Color_EntryNode(0.86f, 0.08f, 0.24f); // 绯红
const FLinearColor UCancerStateMachineSchema::Color_EdgeNode(1.0f, 0.0f, 0.0f);  // 真红

UEdGraphNode* FCancerStateMachineSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2f& Location, bool bSelectNewNode)
{
	if (UCancerStateMachineEdNode_CustomBP* CustomBPNodeTemplate = Cast<UCancerStateMachineEdNode_CustomBP>(NodeTemplate))
	{
		if (CustomBPNodeTemplate->bRequestCreateNewAsset)
		{
			// 执行创建新资产的逻辑
			IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
			UCancerStateMachineNodeFactory* Factory = NewObject<UCancerStateMachineNodeFactory>();
			
			UObject* NewAsset = AssetTools.CreateAssetWithDialog(UCancerStateMachineNodeBlueprint::StaticClass(), Factory);
			if (NewAsset)
			{
				UBlueprint* BP = Cast<UBlueprint>(NewAsset);
				// 直接实例化新创建的蓝图类
				CustomBPNodeTemplate->RuntimeNode = NewObject<UCancerStateMachineNode>(CustomBPNodeTemplate, BP->GeneratedClass);
				CustomBPNodeTemplate->RuntimeNode->SetFlags(RF_Transactional);
				// 自动设置节点名称为资产名称
				CustomBPNodeTemplate->RuntimeNode->NodeName = FText::FromString(NewAsset->GetName());
			}
			else
			{
				// 如果取消创建资产，则不继续创建节点
				return nullptr;
			}
		}
	}

	UCancerStateMachineEdNode* ResultNode = CancerStateMachineSchemaHelpers::CreateNode<UCancerStateMachineEdNode>(
		ParentGraph, FromPins, Location, LOCTEXT("StateMachineNewNode", "State Machine: New Node"), NodeTemplate);

	if (ResultNode)
	{
		// 确保 RuntimeNode 存在（即使是 CustomBP，初始也创建一个基类实例作为占位符存储名称）
		if (!ResultNode->RuntimeNode)
		{
			ResultNode->RuntimeNode = NewObject<UCancerStateMachineNode>(ResultNode);
			ResultNode->RuntimeNode->SetFlags(RF_Transactional);
		}

		FString DefaultName = LOCTEXT("NewStateDefaultName", "New State").ToString();
		FString UniqueName = CancerStateMachineSchemaHelpers::GenerateUniqueNodeName(ParentGraph, DefaultName, ResultNode);
		ResultNode->RuntimeNode->NodeName = FText::FromString(UniqueName);

		// 如果是 Inline 节点，初始化其内部蓝图
		if (UCancerStateMachineEdNode_Inline* InlineNode = Cast<UCancerStateMachineEdNode_Inline>(ResultNode))
		{
			InlineNode->CreateInnerBlueprint();
		}
	}

	return ResultNode;
}

void FCancerStateMachineSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FCancerStateMachineSchemaAction_NewGlobalNode::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2f& Location, bool bSelectNewNode)
{
	return CancerStateMachineSchemaHelpers::CreateNode<UCancerStateMachineEdNodeGlobal>(
		ParentGraph, FromPins, Location, LOCTEXT("StateMachineNewGlobalNode", "State Machine: New Global Node"), NodeTemplate);
}

void FCancerStateMachineSchemaAction_NewGlobalNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FCancerStateMachineSchemaAction_NewEdge::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2f& Location, bool bSelectNewNode)
{
	return CancerStateMachineSchemaHelpers::CreateNode<UCancerStateMachineEdNodeEdge>(
		ParentGraph, FromPins, Location, LOCTEXT("StateMachineNewEdge", "State Machine: New Edge"), NodeTemplate);
}

void FCancerStateMachineSchemaAction_NewEdge::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FCancerStateMachineSchemaAction_NewEventEdge::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2f& Location, bool bSelectNewNode)
{
	return CancerStateMachineSchemaHelpers::CreateNode<UCancerStateMachineEdNodeEventEdge>(
		ParentGraph, FromPins, Location, LOCTEXT("StateMachineNewEventEdge", "State Machine: New Event Edge"), NodeTemplate);
}

void FCancerStateMachineSchemaAction_NewEventEdge::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FCancerStateMachineSchemaAction_AddComment::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2f& Location, bool bSelectNewNode)
{
	// 创建注释节点模板
	UEdGraphNode_Comment* const CommentTemplate = NewObject<UEdGraphNode_Comment>();

	FVector2f SpawnLocation = Location;
	FSlateRect Bounds;

	// 获取图表编辑器实例以计算选中节点的范围
	const TSharedPtr<SGraphEditor> GraphEditorPtr = SGraphEditor::FindGraphEditorForGraph(ParentGraph);
	if (GraphEditorPtr.IsValid() && GraphEditorPtr->GetBoundsForSelectedNodes(/*out*/ Bounds, 50.0f))
	{
		// 如果有选中的节点，将注释框包裹住这些节点
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	// 使用通用的 SpawnNodeFromTemplate 创建节点
	UEdGraphNode* const NewNode = FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation, bSelectNewNode);

	return NewNode;
}

void UCancerStateMachineSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// 1. 添加任务型状态 (Type A)
	{
		TSharedPtr<FCancerStateMachineSchemaAction_NewNode> NewTaskNodeAction(new FCancerStateMachineSchemaAction_NewNode(
			LOCTEXT("StateCategory", "States"),
			LOCTEXT("AddTaskStateMenuDesc", "Add Task State"),
			LOCTEXT("AddTaskStateToolTip", "Create a new task-based state node"),
			0
		));
		NewTaskNodeAction->NodeTemplate = NewObject<UCancerStateMachineEdNode_Task>(ContextMenuBuilder.OwnerOfTemporaries);
		NewTaskNodeAction->NodeTemplate->RuntimeNode = NewObject<UCancerStateMachineNode_Task>(NewTaskNodeAction->NodeTemplate);
		ContextMenuBuilder.AddAction(NewTaskNodeAction);
	}

	// 2. 添加蓝图逻辑型状态 (Type B)
	{
		TSharedPtr<FCancerStateMachineSchemaAction_NewNode> NewBPNodeAction(new FCancerStateMachineSchemaAction_NewNode(
			LOCTEXT("StateCategory", "States"),
			LOCTEXT("AddBPStateMenuDesc", "Add Blueprint State"),
			LOCTEXT("AddBPStateToolTip", "Create a new blueprint-based state node"),
			0
		));
		NewBPNodeAction->NodeTemplate = NewObject<UCancerStateMachineEdNode_CustomBP>(ContextMenuBuilder.OwnerOfTemporaries);
		// Type B 节点初始不创建 RuntimeNode，由用户选择 NodeClass 后实例化
		NewBPNodeAction->NodeTemplate->RuntimeNode = nullptr;
		ContextMenuBuilder.AddAction(NewBPNodeAction);

		// 2.1 快捷创建新蓝图并添加
		TSharedPtr<FCancerStateMachineSchemaAction_NewNode> CreateNewBPAction(new FCancerStateMachineSchemaAction_NewNode(
			LOCTEXT("StateCategory", "States"),
			LOCTEXT("CreateNewBPStateMenuDesc", "Create New Blueprint State..."),
			LOCTEXT("CreateNewBPStateToolTip", "Create a new blueprint asset for state logic and add it as a node"),
			0
		));
		UCancerStateMachineEdNode_CustomBP* BPNodeTemplate = NewObject<UCancerStateMachineEdNode_CustomBP>(ContextMenuBuilder.OwnerOfTemporaries);
		BPNodeTemplate->RuntimeNode = nullptr;
		BPNodeTemplate->bRequestCreateNewAsset = true; // 标记需要创建新资产
		CreateNewBPAction->NodeTemplate = BPNodeTemplate;
		ContextMenuBuilder.AddAction(CreateNewBPAction);
	}

	// 2.2 添加一次性逻辑节点 (Type C)
	{
		TSharedPtr<FCancerStateMachineSchemaAction_NewNode> InlineNodeAction(new FCancerStateMachineSchemaAction_NewNode(
			LOCTEXT("StateCategory", "States"),
			LOCTEXT("AddInlineStateMenuDesc", "Add Inline Logic State"),
			LOCTEXT("AddInlineStateToolTip", "Create a node with its own logic graph that can be later promoted to a class"),
			0
		));
		InlineNodeAction->NodeTemplate = NewObject<UCancerStateMachineEdNode_Inline>(ContextMenuBuilder.OwnerOfTemporaries);
		ContextMenuBuilder.AddAction(InlineNodeAction);
	}

	// 2.3 添加结束状态节点
	{
		TSharedPtr<FCancerStateMachineSchemaAction_NewNode> EndNodeAction(new FCancerStateMachineSchemaAction_NewNode(
			LOCTEXT("StateCategory", "States"),
			LOCTEXT("AddEndStateMenuDesc", "Add End State"),
			LOCTEXT("AddEndStateToolTip", "Create a state node that finishes the state machine when reached"),
			0
		));
		EndNodeAction->NodeTemplate = NewObject<UCancerStateMachineEdNode_End>(ContextMenuBuilder.OwnerOfTemporaries);
		EndNodeAction->NodeTemplate->RuntimeNode = NewObject<UCancerStateMachineNode_End>(EndNodeAction->NodeTemplate);
		ContextMenuBuilder.AddAction(EndNodeAction);
	}

	// 2.4 添加子状态机节点 (HSM)
	{
		TSharedPtr<FCancerStateMachineSchemaAction_NewNode> SubSMNodeAction(new FCancerStateMachineSchemaAction_NewNode(
			LOCTEXT("StateCategory", "States"),
			LOCTEXT("AddSubSMStateMenuDesc", "Add Sub State Machine"),
			LOCTEXT("AddSubSMStateToolTip", "Create a sub-state machine node for Hierarchical State Machines (HSM)"),
			0
		));
		SubSMNodeAction->NodeTemplate = NewObject<UCancerStateMachineEdNode_SubStateMachine>(ContextMenuBuilder.OwnerOfTemporaries);
		SubSMNodeAction->NodeTemplate->RuntimeNode = NewObject<UCancerStateMachineNode_SubStateMachine>(SubSMNodeAction->NodeTemplate);
		ContextMenuBuilder.AddAction(SubSMNodeAction);
	}

	// 3. 添加全局状态节点
	{
		TSharedPtr<FCancerStateMachineSchemaAction_NewGlobalNode> Action(new FCancerStateMachineSchemaAction_NewGlobalNode(
			LOCTEXT("StateCategory", "States"),
			LOCTEXT("AddGlobalStateMenuDesc", "Add Global State"),
			LOCTEXT("AddGlobalStateToolTip", "Create a new global state node"),
			0
		));
		Action->NodeTemplate = NewObject<UCancerStateMachineEdNodeGlobal>(ContextMenuBuilder.OwnerOfTemporaries);
		ContextMenuBuilder.AddAction(Action);
	}

	// 4. 添加条件节点 (边)
	{
		TSharedPtr<FCancerStateMachineSchemaAction_NewEdge> Action(new FCancerStateMachineSchemaAction_NewEdge(
			LOCTEXT("StateCategory", "States"),
			LOCTEXT("AddEdgeMenuDesc", "Add Transition Condition"),
			LOCTEXT("AddEdgeToolTip", "Create a new transition condition between states"),
			0
		));
		Action->NodeTemplate = NewObject<UCancerStateMachineEdNodeEdge>(ContextMenuBuilder.OwnerOfTemporaries);
		Action->NodeTemplate->RuntimeEdge = NewObject<UCancerStateMachineEdge>(Action->NodeTemplate);
		ContextMenuBuilder.AddAction(Action);
	}

	// 4.1 添加事件节点 (边)
	{
		TSharedPtr<FCancerStateMachineSchemaAction_NewEventEdge> Action(new FCancerStateMachineSchemaAction_NewEventEdge(
			LOCTEXT("StateCategory", "States"),
			LOCTEXT("AddEventEdgeMenuDesc", "Add Transition Event (Event)"),
			LOCTEXT("AddEventEdgeToolTip", "Create a new event-driven transition between states"),
			0
		));
		Action->NodeTemplate = NewObject<UCancerStateMachineEdNodeEventEdge>(ContextMenuBuilder.OwnerOfTemporaries);
		Action->NodeTemplate->RuntimeEdge = NewObject<UCancerStateMachineEdge>(Action->NodeTemplate);
		ContextMenuBuilder.AddAction(Action);
	}

	// 5. 添加注释
	{
		TSharedPtr<FCancerStateMachineSchemaAction_AddComment> Action(new FCancerStateMachineSchemaAction_AddComment(
			LOCTEXT("AddCommentMenuDesc", "Add Comment"),
			LOCTEXT("AddCommentToolTip", "Create a new comment node")
		));
		ContextMenuBuilder.AddAction(Action);
	}
}

FLinearColor UCancerStateMachineSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	// 设置引脚颜色为深灰色
	return FLinearColor(0.101961f, 0.101961f, 0.101961f, 1.0f);
}

bool UCancerStateMachineSchema::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	return true;
}

TSharedPtr<FEdGraphSchemaAction> UCancerStateMachineSchema::GetCreateCommentAction() const
{
	return MakeShared<FCancerStateMachineSchemaAction_AddComment>(
		LOCTEXT("AddComment", "添加注释..."),
		LOCTEXT("AddCommentTooltip", "创建一个注释节点以组织图表。")
	);
}

const FPinConnectionResponse UCancerStateMachineSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// 如果引脚为空，通常是正在拖出连线进行预览
	if (!A || !B)
	{
		const UEdGraphPin* DragPin = A ? A : B;
		if (DragPin)
		{
			// 如果是从状态节点、Entry 节点或 Global 节点拖出，允许预览连线（之后会通过转换节点连接）
			if (DragPin->GetOwningNode()->IsA<UCancerStateMachineEdNode>() || 
				DragPin->GetOwningNode()->IsA<UCancerStateMachineEdNodeEntry>() ||
				DragPin->GetOwningNode()->IsA<UCancerStateMachineEdNodeGlobal>())
			{
				if (DragPin->PinName == TEXT("OnFinished"))
				{
					return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnectFinishTransition", "Connect with Completion Transition (Auto)"));
				}
				return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnectTransition", "Connect with Transition"));
			}
		}
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect"));
	}

	// 不允许连接到同一个节点
	UEdGraphNode* NodeA = A->GetOwningNode();
	UEdGraphNode* NodeB = B->GetOwningNode();
	if (NodeA == NodeB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinSameNode", "Cannot connect to the same node"));
	}

	// 不允许连接方向相同的引脚
	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinSameDirection", "Cannot connect pins with same direction"));
	}

	// 状态节点/Entry节点/Global节点 -> 状态节点：需要转换节点（Edge）
	bool bNodeAIsBase = NodeA->IsA<UCancerStateMachineEdNode>() || NodeA->IsA<UCancerStateMachineEdNodeEntry>() || NodeA->IsA<UCancerStateMachineEdNodeGlobal>();
	bool bNodeBIsBase = NodeB->IsA<UCancerStateMachineEdNode>() || NodeB->IsA<UCancerStateMachineEdNodeEntry>() || NodeB->IsA<UCancerStateMachineEdNodeGlobal>();

	if (bNodeAIsBase && bNodeBIsBase)
	{
		// 状态节点之间的连接总是需要一个转换节点
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, LOCTEXT("PinConnectWithEdge", "Connect with Transition"));
	}

	// 节点/Entry -> 转换节点 (Edge)
	bool bNodeAIsEdge = NodeA->IsA<UCancerStateMachineEdNodeEdgeBase>();
	bool bNodeBIsEdge = NodeB->IsA<UCancerStateMachineEdNodeEdgeBase>();

	if (bNodeAIsBase && bNodeBIsEdge)
	{
		// A 是状态，B 是转换节点。转换节点的每个引脚只能有一个连接
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, LOCTEXT("PinConnectEdge", "Connect to Transition"));
	}
	
	if (bNodeBIsBase && bNodeAIsEdge)
	{
		// B 是状态，A 是转换节点
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, LOCTEXT("PinConnectEdge", "Connect to Transition"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinDisallow", "Invalid connection"));
}

bool UCancerStateMachineSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	UEdGraphNode* NodeA = PinA->GetOwningNode();
	UEdGraphNode* NodeB = PinB->GetOwningNode();

	if (NodeA && NodeB)
	{
		// 确定输入和输出引脚
		UEdGraphPin* OutPin = (PinA->Direction == EGPD_Output) ? PinA : PinB;
		UEdGraphPin* InPin = (PinA->Direction == EGPD_Input) ? PinA : PinB;

		// 计算中点位置放置转换节点
		FVector2f Location((NodeA->NodePosX + NodeB->NodePosX) / 2.0f, (NodeA->NodePosY + NodeB->NodePosY) / 2.0f);
		UEdGraph* Graph = NodeA->GetGraph();

		// 创建转换节点
		FCancerStateMachineSchemaAction_NewEdge Action;
		Action.NodeTemplate = NewObject<UCancerStateMachineEdNodeEdge>(Graph);
		Action.NodeTemplate->RuntimeEdge = NewObject<UCancerStateMachineEdge>(Action.NodeTemplate);
		TArray<UEdGraphPin*> Pins;
		UCancerStateMachineEdNodeEdge* EdgeNode = Cast<UCancerStateMachineEdNodeEdge>(Action.PerformAction(Graph, Pins, Location));

		if (EdgeNode)
		{
			// 如果是从 OnFinished 引脚拉出来的，自动标记为 FinishEdge
			if (OutPin->PinName == TEXT("OnFinished"))
			{
				EdgeNode->bIsFinishEdge = true;
				if (EdgeNode->RuntimeEdge)
				{
					EdgeNode->RuntimeEdge->bIsFinishEdge = true;
				}
			}

			// 建立两段连接：起始节点->转换节点，转换节点->目标节点
			TryCreateConnection(OutPin, EdgeNode->GetInputPin());
			TryCreateConnection(EdgeNode->GetOutputPin(), InPin);
			return true;
		}
	}
	return false;
}

void UCancerStateMachineSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// 为新图表创建默认的入口节点 (Entry Node)
	FGraphNodeCreator<UCancerStateMachineEdNodeEntry> NodeCreator(Graph);
	UCancerStateMachineEdNodeEntry* EntryNode = NodeCreator.CreateNode();
	EntryNode->NodePosX = -300;
	EntryNode->NodePosY = 0;
	NodeCreator.Finalize();
}

void UCancerStateMachineSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (Context && Context->Node)
	{
		FToolMenuSection& Section = Menu->AddSection("CancerStateMachineNodeActions", LOCTEXT("NodeActionsMenuHeader", "节点操作"));
		Section.AddMenuEntry(FGenericCommands::Get().Copy);
		Section.AddMenuEntry(FGenericCommands::Get().Cut);
		Section.AddMenuEntry(FGenericCommands::Get().Paste);
		Section.AddMenuEntry(FGenericCommands::Get().Delete);
		Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
	}

	Super::GetContextMenuActions(Menu, Context);
}

#undef LOCTEXT_NAMESPACE
