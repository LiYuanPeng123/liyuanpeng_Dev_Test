

#include "Animation/AnimNotifyState_PostPhase.h"

#include "Types/FCancerCombatAnimNotifyHelpers.h"
#include "Types/FCancerCombatAttackSupport.h"

void UAnimNotifyState_PostPhase::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (auto ASC = GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		ASC->AddLooseGameplayTag(Tag_Combat_State_PostWindow);
	}
}

void UAnimNotifyState_PostPhase::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
}

void UAnimNotifyState_PostPhase::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (auto ASC = GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		ASC->RemoveLooseGameplayTag(Tag_Combat_State_PostWindow);
	}
}
