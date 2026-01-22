#include "Animation/AnimNotifyState_SpecialAbility.h"
#include "CancerAbilityFunctionLibrary.h"
#include "AbilitySystem/Abilities/CombatAbility_ComboAbility.h"
#include "GameFramework/CancerCombo.h"
#include "Types/FCancerCombatAnimNotifyHelpers.h"

void UAnimNotifyState_SpecialAbility::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (auto ASC = UCancerAbilityFunctionLibrary::GetCancerAbilitySystemComponent(MeshComp->GetOwner()))
	{
		FCancerCombatAnimNotifyHelpers::SetTag(MeshComp->GetOwner(), Tag_Combat_State_SpecialAbilityWindow, 1);
		ASC->SetSpecialAbility(SpecialAbility);
		ASC->SetInputTagByAbilityTag(AbilityTag,InputTag);
	}
}

void UAnimNotifyState_SpecialAbility::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (auto ASC = UCancerAbilityFunctionLibrary::GetCancerAbilitySystemComponent(MeshComp->GetOwner()))
	{
		FCancerCombatAnimNotifyHelpers::SetTag(MeshComp->GetOwner(), Tag_Combat_State_SpecialAbilityWindow, 0);
		ASC->SetSpecialAbility(nullptr);
	}
}

FString UAnimNotifyState_SpecialAbility::GetNotifyName_Implementation() const
{
	return TEXT("特殊派生技能");
}
