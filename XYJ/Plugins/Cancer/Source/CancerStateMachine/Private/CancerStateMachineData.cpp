#include "CancerStateMachineData.h"
#include "CancerStateMachineNode.h"
#include "CancerStateMachineEdge.h"
#include "CancerStateMachineCondition.h"

UCancerStateMachineData::UCancerStateMachineData()
{
}

UCancerStateMachineNode* UCancerStateMachineData::GetStartNode(UObject* Context) const
{
	// 1. 优先检查 EntryEdges (条件入口)
	for (UCancerStateMachineEdge* Edge : EntryEdges)
	{
		if (Edge && Edge->EndNode)
		{
			// 如果边没有条件，或者条件满足，则作为启动节点
			if (Edge->Conditions.Num() == 0 || Edge->CanTransition(Context, 0.0f))
			{
				return Edge->EndNode;
			}
		}
	}

	return nullptr;
}

void UCancerStateMachineData::CreateInstance(UObject* Outer, FCancerStateMachineInstance& OutInstance) const
{
	OutInstance.DataAsset = this;
	OutInstance.Owner = Outer;
	OutInstance.RuntimeNodesMap.Empty();
	OutInstance.RuntimeGlobalEdges.Empty();

	// 1. 实例化全局转换 (全局边必须在启动时就存在，以便监听事件)
	for (UCancerStateMachineEdge* EdgeTemplate : GlobalEdges)
	{
		if (EdgeTemplate && EdgeTemplate->EndNode)
		{
			// 确保全局边的目标节点被实例化
			UCancerStateMachineNode* EndInstance = OutInstance.GetOrCreateNodeInstance(EdgeTemplate->EndNode);
			
			UCancerStateMachineEdge* EdgeInstance = DuplicateObject<UCancerStateMachineEdge>(EdgeTemplate, Outer);
			EdgeInstance->StartNode = nullptr;
			EdgeInstance->EndNode = EndInstance;
			EdgeInstance->OwningInstance = &OutInstance;

			// 设置条件的反向引用
			for (UCancerStateMachineCondition* Condition : EdgeInstance->Conditions)
			{
				if (Condition) Condition->OwningEdge = EdgeInstance;
			}

			OutInstance.RuntimeGlobalEdges.Add(EdgeInstance);
		}
	}

	// 2. 对全局边排序
	OutInstance.SortEdges(OutInstance.RuntimeGlobalEdges);
}

UCancerStateMachineNode* FCancerStateMachineInstance::GetOrCreateNodeInstance(UCancerStateMachineNode* TemplateNode)
{
	if (!TemplateNode || !Owner) return nullptr;

	// 1. 如果已经实例化过，直接返回
	if (TObjectPtr<UCancerStateMachineNode>* FoundInstance = RuntimeNodesMap.Find(TemplateNode))
	{
		return *FoundInstance;
	}

	// 2. 实例化新节点
	UCancerStateMachineNode* NodeInstance = DuplicateObject<UCancerStateMachineNode>(TemplateNode, Owner);
	RuntimeNodesMap.Add(TemplateNode, NodeInstance);

	// 3. 延迟修复出边 (OutgoingEdges)
	TArray<TObjectPtr<UCancerStateMachineEdge>> NewEdges;
	for (UCancerStateMachineEdge* EdgeTemplate : NodeInstance->OutgoingEdges)
	{
		if (EdgeTemplate && EdgeTemplate->EndNode)
		{
			// 注意：这里不递归实例化 EndNode，只在真正跳转时才实例化
			// 但我们需要记录这个 EndNode 的模板，以便后续查找
			UCancerStateMachineEdge* EdgeInstance = DuplicateObject<UCancerStateMachineEdge>(EdgeTemplate, NodeInstance);
			EdgeInstance->StartNode = NodeInstance;
			EdgeInstance->EndNode = EdgeTemplate->EndNode; // 暂时指向模板节点
			EdgeInstance->OwningInstance = this;

			// 设置条件的反向引用
			for (UCancerStateMachineCondition* Condition : EdgeInstance->Conditions)
			{
				if (Condition) Condition->OwningEdge = EdgeInstance;
			}

			NewEdges.Add(EdgeInstance);
		}
	}
	NodeInstance->OutgoingEdges = NewEdges;
	SortEdges(NodeInstance->OutgoingEdges);

	return NodeInstance;
}

UCancerStateMachineNode* FCancerStateMachineInstance::CheckTransitions(UObject* Context, UCancerStateMachineNode* CurrentState, float StateTime, bool bIsEventTrigger)
{
	// 1. 检查全局转换 (Global Edges)
	for (UCancerStateMachineEdge* Edge : RuntimeGlobalEdges)
	{
		if (!Edge) continue;

		// 全局边只参与 Tick (或者它们自己绑定的 Event)，目前不参与 NodeCompleted 事件
		if (bIsEventTrigger) continue; 

		if (Edge->EndNode == CurrentState) continue;

		if (Edge->CanTransition(Context, StateTime))
		{
			return GetOrCreateNodeInstance(Edge->EndNode);
		}
	}

	// 2. 检查局部转换 (Local Edges)
	if (!CurrentState) return nullptr;

	for (UCancerStateMachineEdge* Edge : CurrentState->OutgoingEdges)
	{
		if (!Edge) continue;

		// 如果节点已经标记为完成
		if (CurrentState->bIsNodeFinished)
		{
			// 如果是节点完成状态，优先检查连在“完成引脚”上的边
			if (Edge->bIsFinishEdge)
			{
				// 完成引脚不需要条件，直接跳转
				if (Edge->CheckConditions(Context, StateTime))
				{
					return GetOrCreateNodeInstance(Edge->EndNode);
				}
				continue;
			}
		}
		else
		{
			// 如果是每帧 Tick 检查，且节点未完成，跳过“完成引脚”上的边和事件边
			if (Edge->bIsFinishEdge || Edge->bIsEventEdge) continue;

			if (Edge->CheckConditions(Context, StateTime))
			{
				return GetOrCreateNodeInstance(Edge->EndNode);
			}
		}
	}

	return nullptr;
}

void FCancerStateMachineInstance::HandleEventTriggered(UObject* Context, UCancerStateMachineEdge* Edge)
{
	if (Edge && Edge->EndNode)
	{
		// 确保目标节点是实例化的
		UCancerStateMachineNode* TargetInstance = GetOrCreateNodeInstance(Edge->EndNode);
		
		UE_LOG(LogTemp, Log, TEXT("CancerSM: Event Triggered direct transition to [%s]"), *TargetInstance->NodeName.ToString());
		
		OnRequestStateSwitch.Broadcast(TargetInstance);
	}
}

void FCancerStateMachineInstance::SortEdges(TArray<TObjectPtr<UCancerStateMachineEdge>>& Edges)
{
	Edges.Sort([](const TObjectPtr<UCancerStateMachineEdge>& A, const TObjectPtr<UCancerStateMachineEdge>& B)
	{
		if (!A || !B) return false;
		return A->Priority > B->Priority;
	});
}
