// 


#include "Animation/AnimNotifyState_AvoidWindow.h"
#include "CancerNativeGamePlayTag.h"
#include "Types/FCancerCombatAnimNotifyHelpers.h"

void UAnimNotifyState_AvoidWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	FGameplayTag EventTag = Tag_Combat_State_AvoidWindow;
	FCancerCombatAnimNotifyHelpers::SetTag(MeshComp->GetOwner(), EventTag,1);
}

void UAnimNotifyState_AvoidWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	FGameplayTag EventTag = Tag_Combat_State_AvoidWindow;
	FCancerCombatAnimNotifyHelpers::SetTag(MeshComp->GetOwner(), EventTag,0);
}

FString UAnimNotifyState_AvoidWindow::GetNotifyName_Implementation() const
{
	return TEXT("跳踩窗口");
}
