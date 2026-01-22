#include "Combo/ComboMontagePlayer.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

UComboMontagePlayer::UComboMontagePlayer()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UComboMontagePlayer::BeginPlay()
{
	Super::BeginPlay();
	USkeletalMeshComponent* MeshComp = GetOwner() ? GetOwner()->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
	AnimInstance = MeshComp ? MeshComp->GetAnimInstance() : nullptr;
}

bool UComboMontagePlayer::Play(UAnimMontage* Montage, FName Section, float PlayRate, float RootMotionScale)
{
	if (!AnimInstance || !Montage) return false;
	CurrentMontage = Montage;
	CurrentPlayRate = PlayRate;
	CurrentRootMotionScale = RootMotionScale;
	float Len = AnimInstance->Montage_Play(CurrentMontage, CurrentPlayRate);
	if (Len <= 0.f) return false;
	if (Section != NAME_None)
	{
		AnimInstance->Montage_JumpToSection(Section, CurrentMontage);
	}
	OnStarted.Broadcast();
	AnimInstance->OnMontageEnded.AddDynamic(this, &UComboMontagePlayer::HandleMontageEnded);
	return true;
}

void UComboMontagePlayer::JumpToSection(FName Section)
{
	if (AnimInstance && CurrentMontage)
	{
		AnimInstance->Montage_JumpToSection(Section, CurrentMontage);
	}
}

void UComboMontagePlayer::SetPlayRate(float InPlayRate)
{
	CurrentPlayRate = InPlayRate;
	if (AnimInstance && CurrentMontage)
	{
		AnimInstance->Montage_SetPlayRate(CurrentMontage, CurrentPlayRate);
	}
}

void UComboMontagePlayer::SetRootMotionScale(float InScale)
{
	CurrentRootMotionScale = InScale;
}

void UComboMontagePlayer::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UComboMontagePlayer::HandleMontageEnded);
	}
	if (bInterrupted)
	{
		OnCancelled.Broadcast();
	}
	else
	{
		OnEnded.Broadcast();
	}
}
