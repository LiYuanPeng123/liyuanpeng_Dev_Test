#include "CancerStateMachineEdGraph.h"
#include "Nodes/CancerStateMachineEdNode.h"
#include "Nodes/CancerStateMachineEdNodeEdge.h"
#include "Nodes/CancerStateMachineEdNodeEntry.h"
#include "Nodes/CancerStateMachineEdNodeGlobal.h"

void UCancerStateMachineEdGraph::RebuildGraph()
{
	if (!StateMachineData) return;

	// 如果图表已经有节点了，说明序列化已经加载了布局，不需要重构
	if (Nodes.Num() > 0)
	{
		return;
	}

	// 只有在空图表时（比如新建资产），才创建初始 Entry 节点
	FGraphNodeCreator<UCancerStateMachineEdNodeEntry> EntryCreator(*this);
	UCancerStateMachineEdNodeEntry* EntryNode = EntryCreator.CreateNode();
	EntryNode->NodePosX = -300;
	EntryNode->NodePosY = 0;
	EntryCreator.Finalize();
}

void UCancerStateMachineEdGraph::SaveGraph()
{
	if (!StateMachineData) return;

	StateMachineData->Modify();
	StateMachineData->MarkPackageDirty();
	
	// 1. 清理 DataAsset 中的旧引用，准备重新同步
	StateMachineData->AllNodes.Empty();
	StateMachineData->RootNodes.Empty();
	StateMachineData->AllEdges.Empty();
	StateMachineData->EntryEdges.Empty();
	StateMachineData->GlobalEdges.Empty();

	// 2. 同步节点并获取映射表
	TMap<UEdGraphNode*, UObject*> EdToRuntimeMap;
	SyncNodes(EdToRuntimeMap);

	// 3. 构建连接关系
	BuildConnections(EdToRuntimeMap);

	// 4. 确定 RootNodes (没有父节点的节点)
	for (UCancerStateMachineNode* Node : StateMachineData->AllNodes)
	{
		if (Node && Node->ParentNodes.Num() == 0)
		{
			StateMachineData->RootNodes.Add(Node);
		}
	}
}

void UCancerStateMachineEdGraph::SyncNodes(TMap<UEdGraphNode*, UObject*>& OutEdToRuntimeMap)
{
	for (UEdGraphNode* Node : Nodes)
	{
		if (UCancerStateMachineEdNode* EdNode = Cast<UCancerStateMachineEdNode>(Node))
		{
			if (EdNode->RuntimeNode)
			{
				if (EdNode->RuntimeNode->GetOuter() != StateMachineData)
				{
					EdNode->RuntimeNode->Rename(nullptr, StateMachineData, REN_DontCreateRedirectors | REN_DoNotDirty);
				}
				StateMachineData->AllNodes.Add(EdNode->RuntimeNode);
				
				// 清理旧的出边连接，准备重新构建
				EdNode->RuntimeNode->OutgoingEdges.Empty();
				EdNode->RuntimeNode->ChildrenNodes.Empty();
				EdNode->RuntimeNode->ParentNodes.Empty();
				
				// 记录映射
				OutEdToRuntimeMap.Add(EdNode, EdNode->RuntimeNode);
			}
		}
		else if (UCancerStateMachineEdNodeEdgeBase* EdEdge = Cast<UCancerStateMachineEdNodeEdgeBase>(Node))
		{
			if (EdEdge->RuntimeEdge)
			{
				EdEdge->SyncRuntimeConditions(); // 同步编辑器条件到运行时
				EdEdge->RuntimeEdge->Rename(nullptr, StateMachineData, REN_DontCreateRedirectors | REN_DoNotDirty);
				StateMachineData->AllEdges.Add(EdEdge->RuntimeEdge);
				
				// 清理边的状态
				EdEdge->RuntimeEdge->StartNode = nullptr;
				EdEdge->RuntimeEdge->EndNode = nullptr;
				
				OutEdToRuntimeMap.Add(EdEdge, EdEdge->RuntimeEdge);
			}
		}
	}
}

void UCancerStateMachineEdGraph::BuildConnections(const TMap<UEdGraphNode*, UObject*>& EdToRuntimeMap)
{
	for (UEdGraphNode* Node : Nodes)
	{
		// 处理状态节点的出边
		if (UCancerStateMachineEdNode* StartEdNode = Cast<UCancerStateMachineEdNode>(Node))
		{
			UCancerStateMachineNode* StartRuntimeNode = Cast<UCancerStateMachineNode>(EdToRuntimeMap.FindRef(StartEdNode));
			if (!StartRuntimeNode) continue;

			// 处理普通输出引脚
			if (UEdGraphPin* OutPin = StartEdNode->GetOutputPin())
			{
				ProcessOutputPinConnections(OutPin, StartRuntimeNode, EdToRuntimeMap);
			}
			// 处理完成输出引脚
			if (UEdGraphPin* FinishPin = StartEdNode->GetFinishPin())
			{
				ProcessOutputPinConnections(FinishPin, StartRuntimeNode, EdToRuntimeMap);
			}
		}
		// 处理 Entry 节点
		else if (UCancerStateMachineEdNodeEntry* EntryEdNode = Cast<UCancerStateMachineEdNodeEntry>(Node))
		{
			UEdGraphPin* OutPin = EntryEdNode->GetOutputPin();
			if (OutPin)
			{
				for (UEdGraphPin* LinkedPin : OutPin->LinkedTo)
				{
					if (!LinkedPin) continue;
					
					UEdGraphNode* LinkedNode = LinkedPin->GetOwningNode();
					
					// 情况 A：Entry 直接连到状态
					if (UCancerStateMachineEdNode* TargetEdNode = Cast<UCancerStateMachineEdNode>(LinkedNode))
					{
						UCancerStateMachineNode* TargetRuntimeNode = Cast<UCancerStateMachineNode>(EdToRuntimeMap.FindRef(TargetEdNode));
						if (TargetRuntimeNode)
						{
							// 如果没有中间边对象，创建一个默认边
							UCancerStateMachineEdge* DefaultEdge = NewObject<UCancerStateMachineEdge>(StateMachineData);
							DefaultEdge->StartNode = nullptr;
							DefaultEdge->EndNode = TargetRuntimeNode;
							StateMachineData->EntryEdges.Add(DefaultEdge);
							StateMachineData->AllEdges.Add(DefaultEdge);
						}
					}
					// 情况 B：Entry 通过转换节点连接
					else if (UCancerStateMachineEdNodeEdgeBase* EdEdgeNode = Cast<UCancerStateMachineEdNodeEdgeBase>(LinkedNode))
					{
						UCancerStateMachineEdNode* ActualTargetEdNode = EdEdgeNode->GetEndNode();
						if (ActualTargetEdNode && EdEdgeNode->RuntimeEdge)
						{
							UCancerStateMachineNode* TargetRuntimeNode = Cast<UCancerStateMachineNode>(EdToRuntimeMap.FindRef(ActualTargetEdNode));
							if (TargetRuntimeNode)
							{
								EdEdgeNode->SyncRuntimeConditions();
								EdEdgeNode->RuntimeEdge->StartNode = nullptr; // 入口边没有源状态
								EdEdgeNode->RuntimeEdge->EndNode = TargetRuntimeNode;
								StateMachineData->EntryEdges.Add(EdEdgeNode->RuntimeEdge);
							}
						}
					}
				}
			}
		}
		// 处理 Global 节点
		else if (UCancerStateMachineEdNodeGlobal* GlobalEdNode = Cast<UCancerStateMachineEdNodeGlobal>(Node))
		{
			UEdGraphPin* OutPin = GlobalEdNode->GetOutputPin();
			if (OutPin)
			{
				for (UEdGraphPin* LinkedPin : OutPin->LinkedTo)
				{
					if (!LinkedPin) continue;
					
					UEdGraphNode* LinkedNode = LinkedPin->GetOwningNode();
					
					// 情况 A：Global 直接连到状态
					if (UCancerStateMachineEdNode* TargetEdNode = Cast<UCancerStateMachineEdNode>(LinkedNode))
					{
						UCancerStateMachineNode* TargetRuntimeNode = Cast<UCancerStateMachineNode>(EdToRuntimeMap.FindRef(TargetEdNode));
						if (TargetRuntimeNode)
						{
							// 如果没有中间边对象，创建一个默认边
							UCancerStateMachineEdge* DefaultEdge = NewObject<UCancerStateMachineEdge>(StateMachineData);
							DefaultEdge->StartNode = nullptr; // 全局转换没有固定的源状态
							DefaultEdge->EndNode = TargetRuntimeNode;
							StateMachineData->GlobalEdges.Add(DefaultEdge);
							StateMachineData->AllEdges.Add(DefaultEdge);
						}
					}
					// 情况 B：Global 通过转换节点连接
					else if (UCancerStateMachineEdNodeEdgeBase* EdEdgeNode = Cast<UCancerStateMachineEdNodeEdgeBase>(LinkedNode))
					{
						UCancerStateMachineEdNode* ActualTargetEdNode = EdEdgeNode->GetEndNode();
						if (ActualTargetEdNode && EdEdgeNode->RuntimeEdge)
						{
							UCancerStateMachineNode* TargetRuntimeNode = Cast<UCancerStateMachineNode>(EdToRuntimeMap.FindRef(ActualTargetEdNode));
							if (TargetRuntimeNode)
							{
								EdEdgeNode->SyncRuntimeConditions();
								EdEdgeNode->RuntimeEdge->StartNode = nullptr; // 全局转换没有固定的源状态
								EdEdgeNode->RuntimeEdge->EndNode = TargetRuntimeNode;
								StateMachineData->GlobalEdges.Add(EdEdgeNode->RuntimeEdge);
							}
						}
					}
				}
			}
		}
	}
}

void UCancerStateMachineEdGraph::ProcessOutputPinConnections(UEdGraphPin* OutPin, UCancerStateMachineNode* StartRuntimeNode, const TMap<UEdGraphNode*, UObject*>& EdToRuntimeMap)
{
	if (!OutPin) return;
	
	const bool bIsFinishPin = (OutPin->PinName == TEXT("OnFinished"));

	for (UEdGraphPin* LinkedPin : OutPin->LinkedTo)
	{
		UEdGraphNode* LinkedNode = LinkedPin->GetOwningNode();
		
		// 情况 A：直接连到另一个状态
		if (UCancerStateMachineEdNode* EndEdNode = Cast<UCancerStateMachineEdNode>(LinkedNode))
		{
			UCancerStateMachineNode* EndRuntimeNode = Cast<UCancerStateMachineNode>(EdToRuntimeMap.FindRef(EndEdNode));
			if (EndRuntimeNode)
			{
				// 如果没有中间边对象，创建一个默认边
				UCancerStateMachineEdge* DefaultEdge = NewObject<UCancerStateMachineEdge>(StartRuntimeNode);
				DefaultEdge->StartNode = StartRuntimeNode;
				DefaultEdge->EndNode = EndRuntimeNode;
				DefaultEdge->bIsFinishEdge = bIsFinishPin; // 如果是从 OnFinished 拉出来的，标记为完成转换
				StartRuntimeNode->OutgoingEdges.Add(DefaultEdge);
				
				// 更新父子关系 (对齐 ComboGraph)
				StartRuntimeNode->ChildrenNodes.AddUnique(EndRuntimeNode);
				EndRuntimeNode->ParentNodes.AddUnique(StartRuntimeNode);
				
				StateMachineData->AllEdges.Add(DefaultEdge);
			}
		}
		// 情况 B：连到了转换节点 (Edge Node)
		else if (UCancerStateMachineEdNodeEdgeBase* EdEdgeNode = Cast<UCancerStateMachineEdNodeEdgeBase>(LinkedNode))
		{
			UCancerStateMachineEdNode* ActualEndEdNode = EdEdgeNode->GetEndNode();
			UCancerStateMachineEdge* RuntimeEdge = EdEdgeNode->RuntimeEdge;
			
			if (ActualEndEdNode && RuntimeEdge)
			{
				UCancerStateMachineNode* EndRuntimeNode = Cast<UCancerStateMachineNode>(EdToRuntimeMap.FindRef(ActualEndEdNode));
				if (EndRuntimeNode)
				{
					// 如果是从 OnFinished 引脚连出来的，强制同步边属性
					if (bIsFinishPin)
					{
						EdEdgeNode->bIsFinishEdge = true;
						RuntimeEdge->bIsFinishEdge = true;
					}

					RuntimeEdge->StartNode = StartRuntimeNode;
					RuntimeEdge->EndNode = EndRuntimeNode;
					StartRuntimeNode->OutgoingEdges.Add(RuntimeEdge);
					
					// 更新父子关系 (对齐 ComboGraph)
					StartRuntimeNode->ChildrenNodes.AddUnique(EndRuntimeNode);
					EndRuntimeNode->ParentNodes.AddUnique(StartRuntimeNode);
				}
			}
		}
	}
}
