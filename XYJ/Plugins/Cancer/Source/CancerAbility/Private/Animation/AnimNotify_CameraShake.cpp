#include "Animation/AnimNotify_CameraShake.h"
#include "GameFramework/Character.h"

UAnimNotify_CameraShake::UAnimNotify_CameraShake()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(0, 0, 197);
#endif
}

void UAnimNotify_CameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (CameraShake)
				PlayerController->ClientStartCameraShake(CameraShake);
		}
	}
}

FString UAnimNotify_CameraShake::GetNotifyName_Implementation() const
{
	return TEXT("CameraShake");
}
