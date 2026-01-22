#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "ComboGraphEdNode_Move.generated.h"

class UMoveDefinition;

UCLASS()
class CANCERABILITYEDITOR_API UComboGraphEdNode_Move : public UEdGraphNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category="Combo")
	TObjectPtr<UMoveDefinition> Move;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	UEdGraphPin* GetInputPin() const { return InputPin; }
	UEdGraphPin* GetOutputPin() const { return OutputPin; }
private:
	UEdGraphPin* InputPin = nullptr;
	UEdGraphPin* OutputPin = nullptr;
};

