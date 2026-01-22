
#include "Nodes/AGN_BlendByGameplayTags.h"
#include "GameplayTagsManager.h"

#define LOCTEXT_NAMESPACE "AGN_BlendByGameplaytags"

namespace BlendGamePlayTag
{
	FName GetSimpleTagName(const FGameplayTag& Tag)
	{
		const auto TagNode{UGameplayTagsManager::Get().FindTagNode(Tag)};
		return TagNode.IsValid() ? TagNode->GetSimpleTagName() : NAME_None;
	}
}

UAGN_BlendByGameplayTags::UAGN_BlendByGameplayTags()
{
	Node.AddPose();
}

void UAGN_BlendByGameplayTags::PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent)
{
	if (ChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FAN_BlendByGameplayTag, Tags))
	{
		ReconstructNode();
	}

	Super::PostEditChangeProperty(ChangedEvent);
}

FText UAGN_BlendByGameplayTags::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Title", "Blend Poses by Gameplay Tag");
}

FText UAGN_BlendByGameplayTags::GetTooltipText() const
{
	return LOCTEXT("Tooltip", "Blend Poses by Gameplay Tag");
}

void UAGN_BlendByGameplayTags::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& PreviousPins)
{
	Node.RefreshPosePins();
	Super::ReallocatePinsDuringReconstruction(PreviousPins);
}

FString UAGN_BlendByGameplayTags::GetNodeCategory() const
{
	return FString{TEXTVIEW("ASAnim")};
}

void UAGN_BlendByGameplayTags::CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 PinIndex) const
{
	Super::CustomizePinData(Pin, SourcePropertyName, PinIndex);

	bool bBlendPosePin;
	bool bBlendTimePin;
	GetBlendPinProperties(Pin, bBlendPosePin, bBlendTimePin);

	if (!bBlendPosePin && !bBlendTimePin)
	{
		return;
	}

	Pin->PinFriendlyName = PinIndex <= 0
							   ? LOCTEXT("Default", "Default")
							   : PinIndex > Node.Tags.Num()
							   ? LOCTEXT("Invalid", "Invalid")
							   : FText::AsCultureInvariant(BlendGamePlayTag::GetSimpleTagName(Node.Tags[PinIndex - 1]).ToString());

	if (bBlendPosePin)
	{
		static const FTextFormat BlendPosePinFormat{LOCTEXT("Pose", "{PinName} Pose")};

		Pin->PinFriendlyName = FText::Format(BlendPosePinFormat, {{FString{TEXTVIEW("PinName")}, Pin->PinFriendlyName}});
	}
	else if (bBlendTimePin)
	{
		static const FTextFormat BlendTimePinFormat{LOCTEXT("BlendTime", "{PinName} Blend Time")};

		Pin->PinFriendlyName = FText::Format(BlendTimePinFormat, {{FString{TEXTVIEW("PinName")}, Pin->PinFriendlyName}});
	}
}

void UAGN_BlendByGameplayTags::GetBlendPinProperties(const UEdGraphPin* Pin, bool& bBlendPosePin, bool& bBlendTimePin)
{
	const auto PinFullName{Pin->PinName.ToString()};
	const auto SeparatorIndex{PinFullName.Find(TEXTVIEW("_"), ESearchCase::CaseSensitive)};

	if (SeparatorIndex <= 0)
	{
		bBlendPosePin = false;
		bBlendTimePin = false;
		return;
	}

	const auto PinName{PinFullName.Left(SeparatorIndex)};
	bBlendPosePin = PinName == TEXTVIEW("BlendPose");
	bBlendTimePin = PinName == TEXTVIEW("BlendTime");
}

#undef LOCTEXT_NAMESPACE