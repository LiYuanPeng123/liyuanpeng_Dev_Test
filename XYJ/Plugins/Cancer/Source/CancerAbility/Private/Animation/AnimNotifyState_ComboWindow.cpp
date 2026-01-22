#include "Animation/AnimNotifyState_ComboWindow.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Types/FCancerCombatAnimNotifyHelpers.h"
#include "Types/FCancerCombatAttackSupport.h"

void UAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	FGameplayTag EventTag = Tag_Combat_Event_CombatWindow_Start;
	FCancerCombatAnimNotifyHelpers::BroadcastEvent(MeshComp->GetOwner(), EventTag);
}

void UAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	FGameplayTag EventTag = Tag_Combat_Event_CombatWindow_Stop;
	FCancerCombatAnimNotifyHelpers::BroadcastEvent(MeshComp->GetOwner(), EventTag);
}

FString UAnimNotifyState_ComboWindow::GetNotifyName_Implementation() const
{
	return TEXT("连招窗口");
}
