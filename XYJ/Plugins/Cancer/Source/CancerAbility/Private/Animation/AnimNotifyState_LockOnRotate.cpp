#include "Animation/AnimNotifyState_LockOnRotate.h"

#include "CancerAnimationFunctionLibrary.h"
#include "CancerLockingFunctionLibrary.h"
#include "Components/CancerLockingTargetComponent.h"
#include "Components/CancerMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

void UAnimNotifyState_LockOnRotate::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	if (UCancerMovementComponent* Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent<
		UCancerMovementComponent>(OwnerActor))
	{
		Movement->bUseCustomRotation = true;
	}
}

void UAnimNotifyState_LockOnRotate::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	if (UCancerMovementComponent* Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent<
		UCancerMovementComponent>(OwnerActor))
	{
		Movement->bUseCustomRotation = false;
	}
}

void UAnimNotifyState_LockOnRotate::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	UCancerMovementComponent* Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent<
		UCancerMovementComponent>(OwnerActor);

	if (!Movement) return;

	UCancerLockingTargetComponent* LockingComponent = UCancerLockingFunctionLibrary::GetLockingTargetComponent<
		UCancerLockingTargetComponent>(OwnerActor);

	if (!LockingComponent || !LockingComponent->IsLockTarget())
	{
		Movement->bUseCustomRotation = false;
		return;
	}

	AActor* TargetActor = LockingComponent->GetLockTarget();
	if (!TargetActor)
	{
		return;
	}

	FVector OwnerLocation = OwnerActor->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	FVector Direction = TargetLocation - OwnerLocation;
	Direction.Z = 0.f;

	if (Direction.IsNearlyZero())
	{
		return;
	}

	FRotator CurrentRotation = OwnerActor->GetActorRotation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, TargetLocation);
	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, FrameDeltaTime, InterpSpeed);
	if (Movement)
	{
		Movement->bUseCustomRotation = true;
		OwnerActor->SetActorRotation(NewRotation);
	}
}

FString UAnimNotifyState_LockOnRotate::GetNotifyName_Implementation() const
{
	return TEXT("旋转朝向目标");
}
