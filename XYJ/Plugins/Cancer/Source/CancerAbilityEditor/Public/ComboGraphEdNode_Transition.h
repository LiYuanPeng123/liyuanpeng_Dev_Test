#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "GameplayTagContainer.h"
#include "ComboGraphEdNode_Transition.generated.h"

class UMoveDefinition;

UCLASS()
class CANCERABILITYEDITOR_API UComboGraphEdNode_Transition : public UEdGraphNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category="Combo")
	TObjectPtr<UMoveDefinition> From;
	UPROPERTY(EditAnywhere, Category="Combo")
	TObjectPtr<UMoveDefinition> To;
	UPROPERTY(EditAnywhere, Category="Combo")
	FGameplayTagContainer InputTags;
	UPROPERTY(EditAnywhere, Category="Combo")
	FGameplayTagContainer RequiredStateTags;
	UPROPERTY(EditAnywhere, Category="Combo")
	int32 Priority = 10;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	UEdGraphPin* GetInputPin() const { return InPin; }
	UEdGraphPin* GetOutputPin() const { return OutPin; }
private:
	UEdGraphPin* InPin = nullptr;
	UEdGraphPin* OutPin = nullptr;
};

