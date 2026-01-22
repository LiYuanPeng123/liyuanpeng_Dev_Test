// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_Priority.h"

#include <string>

#include "CancerAbilityFunctionLibrary.h"

void UAnimNotifyState_Priority::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (auto ASC = UCancerAbilityFunctionLibrary::GetCancerAbilitySystemComponent(MeshComp->GetOwner()))
	{
		ASC->SetAbilityTagPriorityOverride(AbilityTag, Priority);
	}
}

void UAnimNotifyState_Priority::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	if (auto ASC = UCancerAbilityFunctionLibrary::GetCancerAbilitySystemComponent(MeshComp->GetOwner()))
	{
		ASC->ClearAbilityTagPriorityOverrideWithTag(AbilityTag); 
	}
}

FString UAnimNotifyState_Priority::GetNotifyName_Implementation() const
{
	if (AbilityTag.IsEmpty())
	{
		return TEXT("技能优先级");
	}
	FString TargetTag;
	for (auto Tag : AbilityTag)
	{
		FString CurrentTag = Tag.ToString();
		TArray<FString> StringArray;
		CurrentTag.ParseIntoArray(StringArray , TEXT(".") );
		TargetTag += StringArray.Last();
		TargetTag += " ";
		TargetTag += FString::FromInt(Priority);
		break;
	}
	return TargetTag;
}
