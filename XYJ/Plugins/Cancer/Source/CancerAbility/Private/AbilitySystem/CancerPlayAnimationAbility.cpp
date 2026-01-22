#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CancerAbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystem/Interfaces/CancerCombatAnimationTaskInterface.h"
#include "AbilitySystem/Tasks/AbilityTask_PlayCombatAnimation.h"

void UCancerPlayAnimationAbility::SetupAndPlayAnimation_Implementation(UAnimMontage* Montage, FName SectionName,
                                                                       float PlayRate, float RootMotionScale)
{
	if (!CommitAnimationCost())
	{
		UE_LOG(LogAbilitySystemComponent,Warning,TEXT("提交动作消耗-- 失败"));
		K2_EndAbility();
		return;
	}
	BeforeAnimationReady();
	UAbilityTask* LastTask = AnimationTask;
	AnimationTask = InitializeAnimationTask(Montage, SectionName, PlayRate, RootMotionScale);
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
		K2_EndAbility();
	}
}

bool UCancerPlayAnimationAbility::HasSection(FName SectionName) const
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

void UCancerPlayAnimationAbility::MoveToSection(FName SectionName) const
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

void UCancerPlayAnimationAbility::SetMontagePlayRate(float PlayRate)
{
	if (IsValid(AnimationTask) && AnimationTask->IsA<UAbilityTask_PlayCombatAnimation>())
	{
		const UAbilityTask_PlayCombatAnimation* MontageAnimationTask = Cast<UAbilityTask_PlayCombatAnimation>(
			AnimationTask);
		MontageAnimationTask->SetPlayRate(PlayRate);
	}
}

void UCancerPlayAnimationAbility::ResetMontagePlayRate()
{
	if (IsValid(AnimationTask) && AnimationTask->IsA<UAbilityTask_PlayCombatAnimation>())
	{
		const UAbilityTask_PlayCombatAnimation* MontageAnimationTask = Cast<UAbilityTask_PlayCombatAnimation>(
			AnimationTask);
		MontageAnimationTask->ResetPlayRate();
	}
}

void UCancerPlayAnimationAbility::BeforeAnimationReady_Implementation()
{
}

void UCancerPlayAnimationAbility::OnAnimationReady_Implementation()
{
}

void UCancerPlayAnimationAbility::OnAnimationStarted_Implementation()
{
}

void UCancerPlayAnimationAbility::OnAnimationFinished_Implementation()
{
	if (bMatchAnimationEnd)
		K2_EndAbility();
}

void UCancerPlayAnimationAbility::OnAnimationCancelled_Implementation()
{
	if (bMatchAnimationEnd)
		K2_CancelAbility();
}

UAbilityTask* UCancerPlayAnimationAbility::InitializeAnimationTask_Implementation(UAnimMontage* Montage,
	FName SectionName, float PlayRate, float RootMotionScale)
{
	auto HasSection = [Montage](const FName& SectionName) -> bool
	{
		for (const auto& Sec : Montage->CompositeSections)
		{
			if (Sec.SectionName == SectionName)
			{
				return true;
			}
		}
		return false;
	};
	
	
	if (IsValid(Montage))
	{
		if (!HasSection(SectionName))
		{
		UE_LOG(LogTemp,Warning,TEXT("InitializeAnimationTask:: SectionName not Find !!!"))
			SectionName = NAME_None;
		}
		
		static const FName TaskInstanceName = TEXT("PlayMontage");
		UAbilityTask* Task = UAbilityTask_PlayCombatAnimation::CreatePlayCombatAnimationProxy(
			this, TaskInstanceName, Montage, PlayRate, SectionName, true, RootMotionScale);
		return Task;
	}

	return nullptr;
}

void UCancerPlayAnimationAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             bool bReplicateEndAbility, bool bWasCancelled)
{
	FinishLatentTask(AnimationTask);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCancerPlayAnimationAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (bPlayMontageWhenActivated)
	{
		SetupAndPlayAnimation(GetMontage());
	}
}

void UCancerPlayAnimationAbility::PlayMontageByMontageTag(FGameplayTag TargetAbilityMontageTag)
{
	if (UCancerAbilitySystemComponent* AbilitySystemComponent = GetCancerAbilitySystemComponentFromActorInfo())
	{
		if (UAnimMontage* AnimMontage = AbilitySystemComponent->GetAbilityMontage(TargetAbilityMontageTag))
		{
			SetupAndPlayAnimation(AnimMontage);
		}
	}
}

UAnimMontage* UCancerPlayAnimationAbility::GetMontage_Implementation()
{
	if (auto ASC = GetCancerAbilitySystemComponentFromActorInfo())
	{
		return ASC->GetAbilityMontage(DefaultMontageTag);
	}
	return nullptr;
}
