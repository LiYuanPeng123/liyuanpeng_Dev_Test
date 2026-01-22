#include "Animation/AnimNotifyState_BlockWindow.h"

#include "Types/FCancerCombatAnimNotifyHelpers.h"
#include "Types/FCancerCombatAttackSupport.h"

void UAnimNotifyState_BlockWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	FGameplayTag EventTag = Tag_Combat_State_BlockWindow;
	FCancerCombatAnimNotifyHelpers::SetTag(MeshComp->GetOwner(), EventTag,1);
}

void UAnimNotifyState_BlockWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	FGameplayTag EventTag = Tag_Combat_State_BlockWindow;
	FCancerCombatAnimNotifyHelpers::SetTag(MeshComp->GetOwner(), EventTag,0);
}

FString UAnimNotifyState_BlockWindow::GetNotifyName_Implementation() const
{
	return TEXT("格挡窗口");
}
