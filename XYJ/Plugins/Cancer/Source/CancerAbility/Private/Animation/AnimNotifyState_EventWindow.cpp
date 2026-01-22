// GShenLin


#include "Animation/AnimNotifyState_EventWindow.h"

#include "Types/FCancerCombatAnimNotifyHelpers.h"

void UAnimNotifyState_EventWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	FCancerCombatAnimNotifyHelpers::BroadcastEvent(MeshComp->GetOwner(), BeginEventTag);
}

void UAnimNotifyState_EventWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
											 const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	FCancerCombatAnimNotifyHelpers::BroadcastEvent(MeshComp->GetOwner(), EndEventTag);
}

FString UAnimNotifyState_EventWindow::GetNotifyName_Implementation() const
{
	return EventName;
}