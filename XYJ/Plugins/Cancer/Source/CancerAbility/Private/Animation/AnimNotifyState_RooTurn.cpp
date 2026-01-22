
#include "Animation/AnimNotifyState_RooTurn.h"

#include "CancerAbilityFunctionLibrary.h"
#include "CancerAnimationFunctionLibrary.h"
#include "Components/CancerHeroComponent.h"
#include "Components/CancerMovementComponent.h"

void UAnimNotifyState_RooTurn::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (auto CombatManager = UCancerAbilityFunctionLibrary::GetCancerCombatManagerComponent
		(MeshComp->GetOwner()))
	{
		if (CombatManager->HasCombatTarget())
		{
			if (UCancerMovementComponent* Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent(
				MeshComp->GetOwner()))
			{
				Movement->bAllowPhysicsRotationDuringAnimRootMotion = false;
				Movement->bUseCurveRotation = false;
			}
		}
	}
}

void UAnimNotifyState_RooTurn::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (UCancerMovementComponent* Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent(
		MeshComp->GetOwner()))
	{
		Movement->bAllowPhysicsRotationDuringAnimRootMotion = true;
		Movement->bUseCurveRotation = true;
		Movement->RotationRate.Yaw = RotationYaw;
	}
}

void UAnimNotifyState_RooTurn::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (UCancerMovementComponent* Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent(
		MeshComp->GetOwner()))
	{
		Movement->bAllowPhysicsRotationDuringAnimRootMotion = false;
		Movement->bUseCurveRotation = false;
	}
}

FString UAnimNotifyState_RooTurn::GetNotifyName_Implementation() const
{
	return TEXT("RootTurn");
}
