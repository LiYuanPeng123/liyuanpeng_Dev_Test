#include "AbilitySystem/CancerCombatAnimationAbility.h"

#include "AbilitySystem/Cost/CancerAbilityCost.h"
#include "AbilitySystem/Tasks/AbilityTask_PlayCombatAnimation.h"
#include "Providers/CancerCombatAnimationProvider.h"

FName UCancerCombatAnimationAbility::AnimationProviderName = TEXT("AnimationProvider");

UCancerCombatAnimationAbility::UCancerCombatAnimationAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bPlayMontageWhenActivated = true;
	AnimationProvider = CreateOptionalDefaultSubobject<UCancerCombatAnimationProvider>(AnimationProviderName);
}

void UCancerCombatAnimationAbility::PlayAnimationMontage()
{
	SetupAndPlayAnimation();
}

bool UCancerCombatAnimationAbility::HasSection(FName SectionName) const
{
	if (SectionName == NAME_None)
	{
		return false;
	}

	if (!IsValid(AnimationTask) || !AnimationTask->IsA<UAbilityTask_PlayCombatAnimation>())
	{
		return false;
	}

	const UAbilityTask_PlayCombatAnimation* MontageAnimationTask = Cast<
		UAbilityTask_PlayCombatAnimation>(AnimationTask);
	return MontageAnimationTask->HasSection(SectionName);
}

void UCancerCombatAnimationAbility::MoveToSection(FName SectionName) const
{
	if (SectionName == NAME_None)
	{
		return;
	}

	if (IsValid(AnimationTask) && AnimationTask->IsA<UAbilityTask_PlayCombatAnimation>())
	{
		const UAbilityTask_PlayCombatAnimation* MontageAnimationTask = Cast<UAbilityTask_PlayCombatAnimation>(
			AnimationTask);
		if (HasSection(SectionName))
		{
			MontageAnimationTask->MoveToSection(SectionName);
		}
	}
}

void UCancerCombatAnimationAbility::SetMontagePlayRate(float PlayRate)
{
	if (IsValid(AnimationTask) && AnimationTask->IsA<UAbilityTask_PlayCombatAnimation>())
	{
		const UAbilityTask_PlayCombatAnimation* MontageAnimationTask = Cast<UAbilityTask_PlayCombatAnimation>(
			AnimationTask);
		MontageAnimationTask->SetPlayRate(PlayRate);
	}
}

void UCancerCombatAnimationAbility::ResetMontagePlayRate()
{
	if (IsValid(AnimationTask) && AnimationTask->IsA<UAbilityTask_PlayCombatAnimation>())
	{
		const UAbilityTask_PlayCombatAnimation* MontageAnimationTask = Cast<UAbilityTask_PlayCombatAnimation>(
			AnimationTask);
		MontageAnimationTask->ResetPlayRate();
	}
}


void UCancerCombatAnimationAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bPlayMontageWhenActivated)
	{
		PlayAnimationMontage();
	}
}

void UCancerCombatAnimationAbility::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);
}

void UCancerCombatAnimationAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               bool bReplicateEndAbility, bool bWasCancelled)
{
	FinishLatentTask(AnimationTask);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCancerCombatAnimationAbility::OnAnimationFinished_Implementation()
{
	if (bMatchAnimationEnd)
	{
		K2_EndAbility();
	}
}

void UCancerCombatAnimationAbility::OnAnimationCancelled_Implementation()
{
	if (bMatchAnimationEnd)
	{
		K2_CancelAbility();
	}
}

UAbilityTask* UCancerCombatAnimationAbility::InitializeAnimationTask_Implementation()
{
	if (IsValid(AnimationProvider))
	{
		UAnimMontage* Montage = AnimationProvider->GetMontageToPlay(this);
		const FName SectionName = AnimationProvider->GetSectionName(this);
		const float PlayRate = AnimationProvider->GetPlayRate(this);
		const float RootMotionScale = AnimationProvider->GetRootMotionScale(this);

		if (IsValid(Montage))
		{
			static const FName TaskInstanceName = TEXT("PlayMontage");
			UAbilityTask* Task = UAbilityTask_PlayCombatAnimation::CreatePlayCombatAnimationProxy(
				this, TaskInstanceName, Montage, PlayRate, SectionName, true, RootMotionScale);
			return Task;
		}
	}

	return nullptr;
}

void UCancerCombatAnimationAbility::SetupAndPlayAnimation_Implementation()
{
	if (!CommitAnimationCost())
	{
		UE_LOG(LogAbilitySystemComponent,Warning,TEXT("提交动作消耗-- 失败"));
		K2_EndAbility();
		return;
	}
	UAbilityTask* LastTask = AnimationTask;
	AnimationTask = InitializeAnimationTask();
	if (IsValid(AnimationTask))
	{
		FinishLatentTask(LastTask);
	}
	if (IsValid(AnimationTask) && AnimationTask->Implements<UCancerCombatAnimationTaskInterface>())
	{
		FCombatAnimationEventDelegate ReadyDelegate;
		ReadyDelegate.BindDynamic(this, &ThisClass::OnAnimationReady);

		FCombatAnimationEventDelegate StartedDelegate;
		StartedDelegate.BindDynamic(this, &ThisClass::OnAnimationStarted);

		FCombatAnimationEventDelegate CompletedDelegate;
		CompletedDelegate.BindDynamic(this, &ThisClass::OnAnimationFinished);

		FCombatAnimationEventDelegate CancelledDelegate;
		CancelledDelegate.BindDynamic(this, &ThisClass::OnAnimationCancelled);

		ICancerCombatAnimationTaskInterface::Execute_BindToAnimationReady(AnimationTask, ReadyDelegate);
		ICancerCombatAnimationTaskInterface::Execute_BindToAnimationStarted(AnimationTask, StartedDelegate);
		ICancerCombatAnimationTaskInterface::Execute_BindToAnimationCompleted(AnimationTask, CompletedDelegate);
		ICancerCombatAnimationTaskInterface::Execute_BindToAnimationBlendOut(AnimationTask, CompletedDelegate);
		ICancerCombatAnimationTaskInterface::Execute_BindToAnimationCancelled(AnimationTask, CancelledDelegate);
		ICancerCombatAnimationTaskInterface::Execute_BindToAnimationInterrupted(AnimationTask, CancelledDelegate);

		AnimationTask->ReadyForActivation();
	}
	else
	{
		K2_CancelAbility();
	}
}
