#include "Animation/AnimNotifyState_AddTags.h"

#include "AbilitySystemComponent.h"

void UAnimNotifyState_AddTags::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (auto ASC = GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		ASC->AddLooseGameplayTags(Tags);
	}
}

void UAnimNotifyState_AddTags::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (auto ASC = GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		ASC->RemoveLooseGameplayTags(Tags);
	}
}

void UAnimNotifyState_AddTags::HandleTimeExpired(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::HandleTimeExpired(MeshComp, Animation);
	
}

FString UAnimNotifyState_AddTags::GetNotifyName_Implementation() const
{
	if (Tags.IsEmpty())
	{
		return TEXT("添加标签");
	}
	FString TargetTag;
	for (auto Tag : Tags)
	{
		FString CurrentTag = Tag.ToString();
		TArray<FString> StringArray;
		CurrentTag.ParseIntoArray(StringArray , TEXT(".") );
		TargetTag += StringArray.Last();
		if (!TargetTag.IsEmpty())
		{
			TargetTag += " ";
		}
	}
	return TargetTag;
}
