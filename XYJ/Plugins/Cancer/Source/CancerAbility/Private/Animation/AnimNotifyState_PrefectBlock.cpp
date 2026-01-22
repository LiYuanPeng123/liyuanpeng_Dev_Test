#include "Animation/AnimNotifyState_PrefectBlock.h"
#include "CancerNativeGamePlayTag.h"
#include "Types/FCancerCombatAnimNotifyHelpers.h"

UAnimNotifyState_PrefectBlock::UAnimNotifyState_PrefectBlock()
{
}

void UAnimNotifyState_PrefectBlock::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	FGameplayTag EventTag = Tag_Combat_State_PerfectBlockWindow;
	FCancerCombatAnimNotifyHelpers::SetTag(MeshComp->GetOwner(), EventTag,1);
}

void UAnimNotifyState_PrefectBlock::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	FGameplayTag EventTag = Tag_Combat_State_PerfectBlockWindow;
	FCancerCombatAnimNotifyHelpers::SetTag(MeshComp->GetOwner(), EventTag,0);
}



FString UAnimNotifyState_PrefectBlock::GetNotifyName_Implementation() const
{
	return TEXT("弹反窗口");
}
