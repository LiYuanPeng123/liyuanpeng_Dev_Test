#include "Combo/PredictiveMontageTask.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

UPredictiveMontageTask* UPredictiveMontageTask::PlayPredictiveMontage(UGameplayAbility* OwningAbility, UAnimMontage* Montage, FName Section, float InPlayRate, float InRootMotionScale)
{
	UPredictiveMontageTask* Task = NewAbilityTask<UPredictiveMontageTask>(OwningAbility);
	Task->MontageRef = Montage;
	Task->SectionName = Section;
	Task->PlayRate = InPlayRate;
	Task->RootMotionScale = InRootMotionScale;
	return Task;
}

void UPredictiveMontageTask::Activate()
{
	if (!Ability) { EndTask(); return; }
	if (!MontageRef) { EndTask(); return; }
	AnimInstance = Ability->GetCurrentActorInfo() ? Ability->GetCurrentActorInfo()->GetAnimInstance() : nullptr;
	if (!AnimInstance) { EndTask(); return; }
	float Length = AnimInstance->Montage_Play(MontageRef, PlayRate);
	if (Length <= 0.f) { EndTask(); return; }
	if (SectionName != NAME_None)
	{
		AnimInstance->Montage_JumpToSection(SectionName, MontageRef);
	}
	OnStarted.Broadcast();
	AnimInstance->OnMontageEnded.AddDynamic(this, &UPredictiveMontageTask::HandleMontageEnded);
}

void UPredictiveMontageTask::OnDestroy(bool bInOwnerFinished)
{
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UPredictiveMontageTask::HandleMontageEnded);
	}
	Super::OnDestroy(bInOwnerFinished);
}

void UPredictiveMontageTask::JumpToSection(FName Section)
{
	if (AnimInstance && MontageRef)
	{
		AnimInstance->Montage_JumpToSection(Section, MontageRef);
	}
}

void UPredictiveMontageTask::SetPlayRate(float InPlayRate)
{
	PlayRate = InPlayRate;
	if (AnimInstance && MontageRef)
	{
		AnimInstance->Montage_SetPlayRate(MontageRef, PlayRate);
	}
}

void UPredictiveMontageTask::SetRootMotionScale(float InScale)
{
	RootMotionScale = InScale;
}

void UPredictiveMontageTask::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		OnCancelled.Broadcast();
	}
	else
	{
		OnEnded.Broadcast();
	}
	EndTask();
}
