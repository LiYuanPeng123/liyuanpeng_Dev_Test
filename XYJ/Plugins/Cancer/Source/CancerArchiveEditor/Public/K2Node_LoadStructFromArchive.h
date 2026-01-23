#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_LoadStructFromArchive.generated.h"

UCLASS()
class CANCERARCHIVEEDITOR_API UK2Node_LoadStructFromArchive : public UK2Node
{
	GENERATED_BODY()

public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(class FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual bool IsNodePure() const override { return false; }
	virtual void ReconstructNode() override;
	virtual void PostReconstructNode() override;
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	void RefreshOutputStructType();

	UPROPERTY(EditAnywhere, Category = "CancerArchive")
	UScriptStruct* StructType;
};
