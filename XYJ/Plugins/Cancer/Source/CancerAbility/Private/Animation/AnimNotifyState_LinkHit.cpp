

#include "Animation/AnimNotifyState_LinkHit.h"

#include "CancerNativeGamePlayTag.h"
#include "Types/FCancerCombatAnimNotifyHelpers.h"

void UAnimNotifyState_LinkHit::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	FGameplayTag EventTag = Tag_Combat_Event_LinkHitWindow_Start;
	FCancerCombatAnimNotifyHelpers::BroadcastEvent(MeshComp->GetOwner(), EventTag);
}

void UAnimNotifyState_LinkHit::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	FGameplayTag EventTag = Tag_Combat_Event_LinkHitWindow_End;
	FCancerCombatAnimNotifyHelpers::BroadcastEvent(MeshComp->GetOwner(), EventTag);
}
