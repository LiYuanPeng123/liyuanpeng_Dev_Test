#include "CancerStateMachineSchemaHelpers.h"
#include "Nodes/CancerStateMachineEdNode.h"

namespace CancerStateMachineSchemaHelpers
{
	FString GenerateUniqueNodeName(UEdGraph* Graph, const FString& BaseName, UEdGraphNode* SkipNode)
	{
		FString FinalName = BaseName;
		int32 Suffix = 1;
		bool bNameExists = true;
		while (bNameExists)
		{
			bNameExists = false;
			for (UEdGraphNode* Node : Graph->Nodes)
			{
				if (Node != SkipNode)
				{
					if (UCancerStateMachineEdNode* OtherNode = Cast<UCancerStateMachineEdNode>(Node))
					{
						if (OtherNode->RuntimeNode && OtherNode->RuntimeNode->NodeName.ToString().Equals(FinalName))
						{
							bNameExists = true;
							break;
						}
					}
				}
			}
			if (bNameExists)
			{
				FinalName = FString::Printf(TEXT("%s_%d"), *BaseName, Suffix++);
			}
		}
		return FinalName;
	}
}
