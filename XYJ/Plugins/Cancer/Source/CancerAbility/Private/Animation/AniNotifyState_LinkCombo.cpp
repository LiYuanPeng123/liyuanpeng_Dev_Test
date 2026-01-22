// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AniNotifyState_LinkCombo.h"
#include "CancerNativeGamePlayTag.h"
#include "Providers/LinkCombo/LinkComboProvider.h"
#include "Types/FCancerCombatAnimNotifyHelpers.h"

UAniNotifyState_LinkCombo::UAniNotifyState_LinkCombo()
{
	LinkComboProvider = CreateOptionalDefaultSubobject<ULinkComboProvider>("LinkComboProvider");

#if WITH_EDITORONLY_DATA
	NotifyColor = FColor::Green;
#endif
}

void UAniNotifyState_LinkCombo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	FGameplayTag EventTag = Tag_Combat_Event_ComboLinkWindow_Start;
	FCancerCombatAnimNotifyHelpers::BroadcastEvent(MeshComp->GetOwner(), EventTag, LinkComboProvider);
}

void UAniNotifyState_LinkCombo::HandleTimeExpired(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::HandleTimeExpired(MeshComp, Animation);
	FGameplayTag EventTag = Tag_Combat_Event_ComboLinkWindow_Stop;
	FCancerCombatAnimNotifyHelpers::BroadcastEvent(MeshComp->GetOwner(), EventTag, LinkComboProvider);
}

FString UAniNotifyState_LinkCombo::GetNotifyName_Implementation() const
{
	return TEXT("连招派生招式");
}
