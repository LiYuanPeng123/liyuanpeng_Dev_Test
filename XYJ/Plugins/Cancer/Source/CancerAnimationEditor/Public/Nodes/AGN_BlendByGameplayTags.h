
#pragma once

#include "CoreMinimal.h"
#include "Editor/AnimGraph/Public/AnimGraphNode_Base.h"
#include "Nodes/AN_BlendByGameplayTag.h"
#include "AGN_BlendByGameplayTags.generated.h"


UCLASS(DisplayName= "Blend By GameplayTag")
class CANCERANIMATIONEDITOR_API UAGN_BlendByGameplayTags : public UAnimGraphNode_Base
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FAN_BlendByGameplayTag Node;

public:
	UAGN_BlendByGameplayTags();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent) override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetTooltipText() const override;

	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& PreviousPins) override;

	virtual FString GetNodeCategory() const override;

	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 PinIndex) const override;

protected:
	static void GetBlendPinProperties(const UEdGraphPin* Pin, bool& bBlendPosePin, bool& bBlendTimePin);
	
};
