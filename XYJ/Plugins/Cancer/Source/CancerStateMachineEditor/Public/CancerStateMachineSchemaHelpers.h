#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "ScopedTransaction.h"

class UEdGraph;
class UEdGraphNode;

namespace CancerStateMachineSchemaHelpers
{
	/** 通用的创建节点辅助函数 */
	template<typename TNodeClass>
	TNodeClass* CreateNode(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2f& Location, const FText& TransactionText, TNodeClass* NodeTemplate)
	{
		if (!NodeTemplate) return nullptr;

		const FScopedTransaction Transaction(TransactionText);
		ParentGraph->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, true);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();

		NodeTemplate->NodePosX = FMath::RoundToInt(Location.X);
		NodeTemplate->NodePosY = FMath::RoundToInt(Location.Y);

		for (UEdGraphPin* FromPin : FromPins)
		{
			if (FromPin)
			{
				UEdGraphPin* InPin = NodeTemplate->GetInputPin();
				if (InPin)
				{
					NodeTemplate->GetSchema()->TryCreateConnection(FromPin, InPin);
				}
			}
		}

		return NodeTemplate;
	}

	/** 生成唯一的节点名称 */
	CANCERSTATEMACHINEEDITOR_API FString GenerateUniqueNodeName(UEdGraph* Graph, const FString& BaseName, UEdGraphNode* SkipNode);
}
