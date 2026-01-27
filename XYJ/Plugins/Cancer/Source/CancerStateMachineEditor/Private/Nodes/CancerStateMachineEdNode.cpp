#include "Nodes/CancerStateMachineEdNode.h"
#include "Nodes/CancerStateMachineNode_SubStateMachine.h"
#include "Settings/CancerStateMachineEditorSettings.h"
#include "CancerStateMachineSchemaHelpers.h"
#include "CancerStateMachineTask.h"
#include "CancerStateMachineNodeBlueprint.h"
#include "Factorys/CancerStateMachineNodeFactory.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ScopedTransaction.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Editor.h"
#include "ToolMenus.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "EdGraphUtilities.h"
#include "EdGraphSchema_K2.h"

#define LOCTEXT_NAMESPACE "CancerStateMachineEdNode"

void FInlineBlueprintEditorHelper::OpenAndFocus(UBlueprint* Blueprint)
{
	if (!Blueprint)
	{
		return;
	}

	// 1. 强制为完整蓝图（防止 Data-Only）
	Blueprint->BlueprintType = BPTYPE_Normal;

	// 2. 确保 EventGraph 存在
	UEdGraph* EventGraph = EnsureEventGraph(Blueprint);

	// 3. 编译蓝图（必须在打开前）
	if (Blueprint->Status != BS_UpToDate)
	{
		FKismetEditorUtilities::CompileBlueprint(Blueprint);
	}

	// 4. ⭐ 设置默认打开文档（在 Open 之前）
	if (EventGraph)
	{
		Blueprint->LastEditedDocuments.Empty();
		Blueprint->LastEditedDocuments.Add(EventGraph);
		Blueprint->Modify();
	}

	// 5. 强制调用蓝图编辑器（不再依赖资产推断）
	if (GEditor)
	{
		if (EventGraph)
		{
			// 优先聚焦到具体的图表
			FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(EventGraph);
		}
		else
		{
			// 退而求其次，打开蓝图本身
			FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Blueprint);
		}
	}
}

UEdGraph* FInlineBlueprintEditorHelper::EnsureEventGraph(UBlueprint* Blueprint)
{
	if (!Blueprint)
	{
		return nullptr;
	}

	if (Blueprint->UbergraphPages.Num() > 0)
	{
		return Blueprint->UbergraphPages[0];
	}

	UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(
		Blueprint,
		UEdGraphSchema_K2::GN_EventGraph,
		UEdGraph::StaticClass(),
		UEdGraphSchema_K2::StaticClass());

	if (NewGraph)
	{
		FBlueprintEditorUtils::AddUbergraphPage(Blueprint, NewGraph);
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	}

	return NewGraph;
}

void UCancerStateMachineEdNode::OnRenameNode(const FString& NewName)
{
	if (!RuntimeNode)
	{
		return;
	}

	UEdGraph* Graph = GetGraph();
	UBlueprint* Blueprint = Graph
		? FBlueprintEditorUtils::FindBlueprintForGraph(Graph)
		: nullptr;

	FString FinalName = NewName;
	if (Graph)
	{
		FinalName = CancerStateMachineSchemaHelpers::GenerateUniqueNodeName(
			Graph, NewName, this);
	}

	const FScopedTransaction Transaction(
		LOCTEXT("RenameNode", "Rename Node"));

	Modify();
	RuntimeNode->Modify();

	RuntimeNode->NodeName = FText::FromString(FinalName);

	// ⭐⭐ 关键：直接标记 Blueprint
	if (Blueprint)
	{
		Blueprint->Modify();

		// 告诉 UE：这是“结构/属性级别变化”
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	}

	if (Graph)
	{
		Graph->NotifyGraphChanged();
	}
}

FText UCancerStateMachineEdNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (RuntimeNode && !RuntimeNode->NodeName.IsEmpty())
	{
		return RuntimeNode->NodeName;
	}

	return Super::GetNodeTitle(TitleType);
}

FLinearColor UCancerStateMachineEdNode::GetNodeTitleColor() const
{
	return UCancerStateMachineEditorSettings::Get()->DefaultStateNodeColor;
}

void UCancerStateMachineEdNode::OnNodeDoubleClicked()
{
	// 默认行为：如果有关联的蓝图资产，则打开它
	if (RuntimeNode)
	{
		UClass* TargetClass = RuntimeNode->GetClass();
		UBlueprint* Blueprint = Cast<UBlueprint>(TargetClass->ClassGeneratedBy);
		if (Blueprint)
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Blueprint);
		}
	}
}


#if WITH_EDITOR
void UCancerStateMachineEdNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!PropertyChangedEvent.Property) return;

	FName PropertyName = PropertyChangedEvent.Property->GetFName();
	FName MemberPropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	// 1. 如果修改了 RuntimeNode 本身 (例如用户切换了类)
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UCancerStateMachineEdNode, RuntimeNode))
	{
		if (RuntimeNode)
		{
			RuntimeNode->SetFlags(RF_Transactional);
			OnRenameNode(RuntimeNode->NodeName.ToString());
		}
		
		if (GetGraph())
		{
			GetGraph()->NotifyGraphChanged();
		}
	}
	// 2. 方案 A：监听 RuntimeNode 内部属性的变化 (特别是 NodeName)
	// 当用户在细节面板修改 NodeName 时，MemberPropertyName 通常是 RuntimeNode
	else if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UCancerStateMachineEdNode, RuntimeNode))
	{
		if (PropertyName == TEXT("NodeName"))
		{
			// 确保名称唯一性
			FString UniqueName = CancerStateMachineSchemaHelpers::GenerateUniqueNodeName(GetGraph(), RuntimeNode->NodeName.ToString(), this);
			RuntimeNode->NodeName = FText::FromString(UniqueName);

			// 官方流派：标记蓝图结构性修改，确保所有 UI（包括图表上的标题）刷新
			if (UEdGraph* Graph = GetGraph())
			{
				UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);
				if (Blueprint)
				{
					FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
				}
			}
		}
	}
}

bool UCancerStateMachineEdNode::CanEditChange(const FProperty* InProperty) const
{
	return Super::CanEditChange(InProperty);
}

bool UCancerStateMachineEdNode_Task::CanEditChange(const FProperty* InProperty) const
{
	bool bCanEdit = Super::CanEditChange(InProperty);

	if (InProperty && InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UCancerStateMachineEdNode, RuntimeNode))
	{
		// 对于 Task 节点，锁定类选择器，不允许更改 RuntimeNode 的类
		// 但这不会影响 RuntimeNode 内部属性的编辑
		return false;
	}

	return bCanEdit;
}

// --- UCancerStateMachineEdNode_SubStateMachine ---

FText UCancerStateMachineEdNode_SubStateMachine::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("SubSMNodeMenuTitle", "SubSM 状态 (子状态机)");
	}

	FText BaseTitle = Super::GetNodeTitle(TitleType);

	if (TitleType == ENodeTitleType::EditableTitle)
	{
		return BaseTitle;
	}

	FText TaggedTitle = FText::Format(LOCTEXT("SubSMNodeTitleTag", "SubSM: {0}"), BaseTitle);

	if (UCancerStateMachineNode_SubStateMachine* SubSMNode = Cast<UCancerStateMachineNode_SubStateMachine>(RuntimeNode))
	{
		if (SubSMNode->SubStateMachineData)
		{
			return FText::Format(
				LOCTEXT("SubSMNodeTitleFormat", "{0}\n<{1}>"), TaggedTitle,
				FText::FromString(SubSMNode->SubStateMachineData->GetName()));
		}
	}

	return TaggedTitle;
}

FLinearColor UCancerStateMachineEdNode_SubStateMachine::GetNodeTitleColor() const
{
	return UCancerStateMachineEditorSettings::Get()->SubStateMachineNodeColor;
}

void UCancerStateMachineEdNode_SubStateMachine::OnNodeDoubleClicked()
{
	if (UCancerStateMachineNode_SubStateMachine* SubSMNode = Cast<UCancerStateMachineNode_SubStateMachine>(RuntimeNode))
	{
		if (SubSMNode->SubStateMachineData)
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(SubSMNode->SubStateMachineData);
		}
	}
}

#endif

void UCancerStateMachineEdNode::GetNodeContextMenuActions(class UToolMenu* Menu,
                                                          class UGraphNodeContextMenuContext* Context) const
{
	// TODO: 添加特定的上下文菜单项
}

FText UCancerStateMachineEdNode_Task::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("TaskNodeMenuTitle", "Task 状态 (任务型)");
	}

	FText BaseTitle = Super::GetNodeTitle(TitleType);

	if (TitleType != ENodeTitleType::EditableTitle)
	{
		FText TaggedTitle = FText::Format(LOCTEXT("TaskNodeTitleTag", "Task: {0}"), BaseTitle);
		if (UCancerStateMachineNode_Task* TaskNode = Cast<UCancerStateMachineNode_Task>(RuntimeNode))
		{
			if (TaskNode->StateTasks.Num() > 0)
			{
				FString TaskSummary;
				for (int32 i = 0; i < TaskNode->StateTasks.Num(); ++i)
				{
					if (TaskNode->StateTasks[i])
					{
						if (!TaskSummary.IsEmpty()) TaskSummary += TEXT(", ");
						TaskSummary += TaskNode->StateTasks[i]->GetClass()->GetName().LeftChop(5); // 移除 Task 后缀
					}
				}
				return FText::Format(
					LOCTEXT("TaskNodeTitleFormat", "{0}\n[{1}]"), TaggedTitle, FText::FromString(TaskSummary));
			}
		}
		return TaggedTitle;
	}

	return BaseTitle;
}

FLinearColor UCancerStateMachineEdNode_Task::GetNodeTitleColor() const
{
	return UCancerStateMachineEditorSettings::Get()->TaskNodeColor;
}

// --- UCancerStateMachineEdNode_CustomBP ---

FText UCancerStateMachineEdNode_CustomBP::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("CustomBPNodeMenuTitle", "BP 状态 (蓝图逻辑)");
	}

	FText BaseTitle = Super::GetNodeTitle(TitleType);

	if (TitleType == ENodeTitleType::EditableTitle)
	{
		return BaseTitle;
	}

	FText TaggedTitle = FText::Format(LOCTEXT("CustomBPNodeTitleTag", "BP: {0}"), BaseTitle);

	if (RuntimeNode)
	{
		FString ClassName = RuntimeNode->GetClass()->GetName();
		ClassName.RemoveFromEnd(TEXT("_C"));
		return FText::Format(
			LOCTEXT("CustomBPNodeTitleFormat", "{0}\n({1})"), TaggedTitle, FText::FromString(ClassName));
	}

	return FText::Format(LOCTEXT("CustomBPNodeNoClassFormat", "{0}\n(None)"), TaggedTitle);
}

FLinearColor UCancerStateMachineEdNode_CustomBP::GetNodeTitleColor() const
{
	return UCancerStateMachineEditorSettings::Get()->CustomBPNodeColor;
}

void UCancerStateMachineEdNode_CustomBP::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
}

// --- UCancerStateMachineEdNode_Inline ---

UCancerStateMachineEdNode_Inline::UCancerStateMachineEdNode_Inline()
{
}

void UCancerStateMachineEdNode_Inline::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
}

FText UCancerStateMachineEdNode_Inline::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("InlineNodeMenuTitle", "Inline 状态 (内联逻辑)");
	}

	FText BaseTitle = Super::GetNodeTitle(TitleType);

	if (TitleType == ENodeTitleType::EditableTitle)
	{
		return BaseTitle;
	}

	return FText::Format(LOCTEXT("InlineNodeTitleTag", "Inline: {0}"), BaseTitle);
}

FLinearColor UCancerStateMachineEdNode_Inline::GetNodeTitleColor() const
{
	return UCancerStateMachineEditorSettings::Get()->InlineNodeColor;
}

void UCancerStateMachineEdNode_Inline::OnNodeDoubleClicked()
{
	if (!InnerBlueprint)
	{
		CreateInnerBlueprint();
	}
	FInlineBlueprintEditorHelper::OpenAndFocus(InnerBlueprint);
}

void UCancerStateMachineEdNode_Inline::CreateInnerBlueprint()
{
	if (InnerBlueprint) return;

	// 关键改进：将 Outer 设置为 Package，使其更像一个独立资产
	UObject* OuterPackage = GetOutermost();

	// 生成一个基于当前节点 ID 的唯一名称
	FName UniqueBPName = *FString::Printf(TEXT("%s_Internal_%s"), *GetName(), *NodeGuid.ToString().Left(8));

	InnerBlueprint = Cast<UCancerStateMachineNodeBlueprint>(FKismetEditorUtilities::CreateBlueprint(
		UCancerStateMachineNode::StaticClass(),
		OuterPackage,
		UniqueBPName,
		BPTYPE_Normal,
		UCancerStateMachineNodeBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass()
	));

	if (InnerBlueprint)
	{
		// 设置 RF_Public 和 RF_Standalone 标志，这有助于编辑器将其视为一个“真实”的资产
		InnerBlueprint->SetFlags(RF_Public | RF_Standalone | RF_Transactional);
		InnerBlueprint->BlueprintType = BPTYPE_Normal;

		// 确保有事件图表
		if (InnerBlueprint->UbergraphPages.Num() == 0)
		{
			UEdGraph* EventGraph = FBlueprintEditorUtils::CreateNewGraph(
				InnerBlueprint,
				UEdGraphSchema_K2::GN_EventGraph,
				UEdGraph::StaticClass(),
				UEdGraphSchema_K2::StaticClass());

			if (EventGraph)
			{
				FBlueprintEditorUtils::AddUbergraphPage(InnerBlueprint, EventGraph);

				int32 NodeY = 0;
				TArray<UFunction*> Funs;

				// 添加默认事件节点
				if (UFunction* OnActivatedFunc = UCancerStateMachineNode::StaticClass()->FindFunctionByName(
					GET_FUNCTION_NAME_CHECKED(UCancerStateMachineNode, OnActivated))) Funs.Add(OnActivatedFunc);
				if (UFunction* OnDeactivatedFunc = UCancerStateMachineNode::StaticClass()->FindFunctionByName(
					GET_FUNCTION_NAME_CHECKED(UCancerStateMachineNode, OnDeactivated))) Funs.Add(OnDeactivatedFunc);
				if (UFunction* OnUpdateFunc = UCancerStateMachineNode::StaticClass()->FindFunctionByName(
					GET_FUNCTION_NAME_CHECKED(UCancerStateMachineNode, OnUpdate))) Funs.Add(OnUpdateFunc);

				for (auto Fun : Funs)
				{
					if (Fun)
					{
						FKismetEditorUtilities::AddDefaultEventNode(InnerBlueprint, EventGraph, Fun->GetFName(),
						                                            UCancerStateMachineNode::StaticClass(), NodeY);
						NodeY += 150; // 间距拉大一点
					}
				}

				FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(InnerBlueprint);
			}
		}

		// 初始编译
		FKismetEditorUtilities::CompileBlueprint(InnerBlueprint);

		// 监听编译完成，以便刷新 RuntimeNode
		InnerBlueprint->OnCompiled().AddUObject(this, &UCancerStateMachineEdNode_Inline::OnInnerBlueprintCompiled);

		// 初始同步
		RefreshRuntimeNode();
	}
}

void UCancerStateMachineEdNode_Inline::RefreshRuntimeNode()
{
	if (InnerBlueprint && InnerBlueprint->GeneratedClass)
	{
		// 检查当前的 RuntimeNode 是否已经是正确的类
		if (!RuntimeNode || RuntimeNode->GetClass() != InnerBlueprint->GeneratedClass)
		{
			FText OldName = RuntimeNode ? RuntimeNode->NodeName : FText::GetEmpty();

			Modify();
			RuntimeNode = NewObject<UCancerStateMachineNode>(this, InnerBlueprint->GeneratedClass);

			if (!OldName.IsEmpty())
			{
				RuntimeNode->NodeName = OldName;
			}
		}
	}
}

void UCancerStateMachineEdNode_Inline::OnInnerBlueprintCompiled(UBlueprint* BP)
{
	RefreshRuntimeNode();
}

void UCancerStateMachineEdNode_Inline::DestroyNode()
{
	if (InnerBlueprint)
	{
		InnerBlueprint->OnCompiled().RemoveAll(this);
	}
	Super::DestroyNode();
}

void UCancerStateMachineEdNode_Inline::GetNodeContextMenuActions(class UToolMenu* Menu,
                                                                 class UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	FToolMenuSection& Section = Menu->AddSection("CancerInlineNode", LOCTEXT("InlineNodeHeader", "Inline Logic"));
	Section.AddMenuEntry(
		"PromoteToAsset",
		LOCTEXT("PromoteToAsset", "Promote to Reusable Blueprint..."),
		LOCTEXT("PromoteToAssetTooltip",
		        "Convert this inline logic into a reusable Blueprint asset in Content Browser"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Save"),
		FUIAction(FExecuteAction::CreateUObject(const_cast<UCancerStateMachineEdNode_Inline*>(this),
		                                        &UCancerStateMachineEdNode_Inline::OnPromoteToAsset))
	);
}

void UCancerStateMachineEdNode_Inline::OnPromoteToAsset()
{
	if (!InnerBlueprint)
	{
		CreateInnerBlueprint();
	}

	// 1. 获取目标路径和名称
	IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

	FString DefaultPath = "/Game/States";
	FString DefaultName = RuntimeNode
		                      ? RuntimeNode->NodeName.ToString().Replace(TEXT(" "), TEXT("_"))
		                      : TEXT("NewStateBlueprint");
	if (DefaultName.IsEmpty()) DefaultName = TEXT("NewStateBlueprint");

	FString PackagePath, AssetName;
	AssetTools.CreateUniqueAssetName(DefaultPath / DefaultName, "", PackagePath, AssetName);

	// 使用对话框让用户选择保存位置
	UCancerStateMachineNodeFactory* Factory = NewObject<UCancerStateMachineNodeFactory>();
	UObject* NewAsset = AssetTools.CreateAssetWithDialog(AssetName, FPackageName::GetLongPackagePath(PackagePath),
	                                                     UCancerStateMachineNodeBlueprint::StaticClass(), Factory);

	if (NewAsset)
	{
		UCancerStateMachineNodeBlueprint* NewBP = Cast<UCancerStateMachineNodeBlueprint>(NewAsset);
		if (NewBP)
		{
			// 使用事务保证操作可撤销
			FScopedTransaction Transaction(LOCTEXT("PromoteToAssetTransaction", "Promote Inline State to Asset"));

			// 2. 迁移逻辑：将 InnerBlueprint 的内容移动到新蓝图
			NewBP->Modify();
			NewBP->UbergraphPages.Empty();

			// 复制图表
			for (UEdGraph* OldGraph : InnerBlueprint->UbergraphPages)
			{
				UEdGraph* NewGraph = FEdGraphUtilities::CloneGraph(OldGraph, NewBP, nullptr, true);
				NewBP->UbergraphPages.Add(NewGraph);
			}

			// 复制函数、宏等
			for (UEdGraph* OldGraph : InnerBlueprint->FunctionGraphs)
			{
				UEdGraph* NewGraph = FEdGraphUtilities::CloneGraph(OldGraph, NewBP, nullptr, true);
				NewBP->FunctionGraphs.Add(NewGraph);
			}

			// 编译新蓝图
			FKismetEditorUtilities::CompileBlueprint(NewBP);

			// 3. 替换当前节点为 CustomBP 节点
			UEdGraph* Graph = GetGraph();
			if (Graph)
			{
				Graph->Modify();

				// 获取主蓝图，用于后续标记修改
				UBlueprint* MainBP = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);

				// 创建新节点
				UCancerStateMachineEdNode_CustomBP* NewNode = NewObject<UCancerStateMachineEdNode_CustomBP>(Graph);
				NewNode->CreateNewGuid();
				NewNode->SetFlags(RF_Transactional);
				NewNode->NodePosX = NodePosX;
				NewNode->NodePosY = NodePosY;

				// 设置 RuntimeNode，并确保它使用新资产的类
				NewNode->RuntimeNode = NewObject<UCancerStateMachineNode>(NewNode, NewBP->GeneratedClass);
				NewNode->RuntimeNode->SetFlags(RF_Transactional);
				if (RuntimeNode)
				{
					NewNode->RuntimeNode->NodeName = RuntimeNode->NodeName;
					NewNode->RuntimeNode->StateTag = RuntimeNode->StateTag;
				}

				Graph->AddNode(NewNode, true, true); // 自动选择新节点
				NewNode->AllocateDefaultPins();

				// 迁移连接
				TArray<UEdGraphPin*> OldPins = Pins;
				for (UEdGraphPin* OldPin : OldPins)
				{
					UEdGraphPin* NewPin = NewNode->FindPin(OldPin->PinName, OldPin->Direction);
					if (NewPin)
					{
						TArray<UEdGraphPin*> LinkedPins = OldPin->LinkedTo;
						for (int32 LinkIndex = LinkedPins.Num() - 1; LinkIndex >= 0; --LinkIndex)
						{
							UEdGraphPin* OtherPin = LinkedPins[LinkIndex];
							OldPin->BreakLinkTo(OtherPin);
							NewPin->MakeLinkTo(OtherPin);
						}
					}
				}

				// 删除旧节点
				Graph->RemoveNode(this);

				// 关键：标记主蓝图已修改，确保 UI 刷新和保存提示
				if (MainBP)
				{
					FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainBP);
				}

				// 标记图表已更改
				Graph->NotifyGraphChanged();
			}
		}
	}
}

bool UCancerStateMachineEdNode_Inline::CanEditChange(const FProperty* InProperty) const
{
	bool bCanEdit = Super::CanEditChange(InProperty);

	if (InProperty && InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UCancerStateMachineEdNode, RuntimeNode))
	{
		// 对于 Inline 节点，不允许手动更改 RuntimeNode，因为它是由内部蓝图管理的
		return false;
	}

	return bCanEdit;
}

#undef LOCTEXT_NAMESPACE
