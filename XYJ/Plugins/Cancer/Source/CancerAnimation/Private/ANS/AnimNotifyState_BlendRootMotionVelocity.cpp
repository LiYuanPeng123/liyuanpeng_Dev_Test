

#include "ANS/AnimNotifyState_BlendRootMotionVelocity.h"

#include "CancerAnimationFunctionLibrary.h"
#include "Components/CancerMovementComponent.h"


void UAnimNotifyState_BlendRootMotionVelocity::NotifyBegin(USkeletalMeshComponent* MeshComp,
                                                           UAnimSequenceBase* Animation, float TotalDuration,
                                                           const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (auto movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent(MeshComp->GetOwner()))
	{
		movement->RootMotionBlendAlpha = Blend;
		movement->RootMotionScale = Scale;
	}
}

void UAnimNotifyState_BlendRootMotionVelocity::NotifyEnd(USkeletalMeshComponent* MeshComp,
                                                         UAnimSequenceBase* Animation,
                                                         const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (auto movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent(MeshComp->GetOwner()))
	{
		movement->RootMotionBlendAlpha = 1;
		movement->RootMotionScale = 1;
	}
}
