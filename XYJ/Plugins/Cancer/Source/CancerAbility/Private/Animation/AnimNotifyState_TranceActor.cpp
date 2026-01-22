
#include "Animation/AnimNotifyState_TranceActor.h"
#include "Components/CancerCombatAttributeComponent.h"
#include "Types/FCancerCombatAnimNotifyHelpers.h"

void UAnimNotifyState_TranceActor::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	FGameplayTag EventTag = Tag_Combat_Event_TargetingPreset_Start;
	FCancerCombatAnimNotifyHelpers::BroadcastEvent(MeshComp->GetOwner(), EventTag);
}


void UAnimNotifyState_TranceActor::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	FGameplayTag EventTag = Tag_Combat_Event_TargetingPreset_Stop;
	FCancerCombatAnimNotifyHelpers::BroadcastEvent(MeshComp->GetOwner(), EventTag);
}
