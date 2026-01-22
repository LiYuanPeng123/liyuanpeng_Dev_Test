

#include "ANS/AnimNotifyState_TurnSpeed.h"

#include "CancerAnimationFunctionLibrary.h"
#include "Components/CancerMovementComponent.h"

void UAnimNotifyState_TurnSpeed::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (auto OwnerCharacter = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (auto Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent(OwnerCharacter))
		{
			//Movement->bUseCurveRotation = true;
		}
	}
}

void UAnimNotifyState_TurnSpeed::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (auto OwnerCharacter = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (auto Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent(OwnerCharacter))
		{
			//Movement->bUseCurveRotation = false;
		}
	}
}

FString UAnimNotifyState_TurnSpeed::GetNotifyName_Implementation() const
{
	return "旋转速率";
}


void UAnimNotifyState_TurnSpeed::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (auto OwnerCharacter = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		/*if (auto Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent(OwnerCharacter))
		{
			Movement->RotationRate.Yaw = UCancerAnimationFunctionLibrary::GetAnimationCurveValueFromCharacter(
				OwnerCharacter, TurnSpeedCurve);
		}*/
	}
}
