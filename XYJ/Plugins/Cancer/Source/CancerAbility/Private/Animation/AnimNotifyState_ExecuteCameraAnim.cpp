

#include "Animation/AnimNotifyState_ExecuteCameraAnim.h"

#include "EngineCamerasSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotifyState_ExecuteCameraAnim::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                     float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (!CameraAnimationSequence)
		return;
	if (APawn* Pawn = Cast<APawn>(MeshComp->GetOwner()))
	{
		if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
		{
			if (auto CameraSubSystem = UEngineCamerasSubsystem::GetEngineCamerasSubsystem(PC->GetWorld()))
			{
				CameraSubSystem->PlayCameraAnimation(PC, CameraAnimationSequence, CameraParams);
			}
		}
	}
}

void UAnimNotifyState_ExecuteCameraAnim::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                   const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (APawn* Pawn = Cast<APawn>(MeshComp->GetOwner()))
	{
		if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
		{
			if (auto CameraSubSystem = UEngineCamerasSubsystem::GetEngineCamerasSubsystem(PC->GetWorld()))
			{
				CameraSubSystem->StopAllCameraAnimationsOf(PC, CameraAnimationSequence, bImmediate);
			}
		}
	}
}

FString UAnimNotifyState_ExecuteCameraAnim::GetNotifyName_Implementation() const
{
	return TEXT("执行相机动画");
}
