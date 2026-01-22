#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "ComboGraphEdNode_Entry.generated.h"

UCLASS()
class CANCERABILITYEDITOR_API UComboGraphEdNode_Entry : public UEdGraphNode
{
	GENERATED_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	UEdGraphPin* GetOutputPin() const { return OutPin; }
private:
	UEdGraphPin* OutPin = nullptr;
};

