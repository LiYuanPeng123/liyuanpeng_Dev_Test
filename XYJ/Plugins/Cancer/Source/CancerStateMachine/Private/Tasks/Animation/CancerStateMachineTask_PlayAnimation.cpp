#include "Tasks/Animation/CancerStateMachineTask_PlayAnimation.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

UCancerStateMachineTask_PlayAnimation::UCancerStateMachineTask_PlayAnimation(
	const FObjectInitializer& ObjectInitializer)
		:Super(ObjectInitializer)
{
	PlayRate = 1.0f;
	StartingPosition = 0.0f;
	StartingSection = NAME_None;
}

void UCancerStateMachineTask_PlayAnimation::Activate()
{
	Super::Activate();

	if (!MontageToPlay) return;

	ACharacter* Character = Cast<ACharacter>(GetContext());
	if (!Character) return;

	UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::MontageLength, StartingPosition);
		if (StartingSection != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(StartingSection, MontageToPlay);
		}
	}
}

void UCancerStateMachineTask_PlayAnimation::OnDestroy(bool bInOwnerFinished)
{
	if (MontageToPlay)
	{
		ACharacter* Character = Cast<ACharacter>(GetContext());
		if (Character)
		{
			UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
			if (AnimInstance && AnimInstance->Montage_IsPlaying(MontageToPlay))
			{
				AnimInstance->Montage_Stop(0.25f, MontageToPlay);
			}
		}
	}

	Super::OnDestroy(bInOwnerFinished);
}
