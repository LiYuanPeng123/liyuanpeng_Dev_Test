
#include "AbilitySystem/Tasks/AbilityTask_PlayCombatAnimation.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "CancerAbilityFunctionLibrary.h"
#include "CancerCoreFunctionLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Runtime/Launch/Resources/Version.h"


void UAbilityTask_PlayCombatAnimation::BindToAnimationReady_Implementation(
	const FCombatAnimationEventDelegate& Delegate)
{
	OnCombatAnimationReady.AddUnique(Delegate);
}

void UAbilityTask_PlayCombatAnimation::BindToAnimationStarted_Implementation(
	const FCombatAnimationEventDelegate& Delegate)
{
	OnCombatAnimationStarted.AddUnique(Delegate);
}

void UAbilityTask_PlayCombatAnimation::BindToAnimationCompleted_Implementation(
	const FCombatAnimationEventDelegate& Delegate)
{
	OnCombatAnimationCompleted.AddUnique(Delegate);
}

void UAbilityTask_PlayCombatAnimation::BindToAnimationBlendOut_Implementation(
	const FCombatAnimationEventDelegate& Delegate)
{
	OnCombatAnimationBlendOut.AddUnique(Delegate);
}

void UAbilityTask_PlayCombatAnimation::BindToAnimationCancelled_Implementation(
	const FCombatAnimationEventDelegate& Delegate)
{
	OnCombatAnimationCancelled.AddUnique(Delegate);
}

void UAbilityTask_PlayCombatAnimation::BindToAnimationInterrupted_Implementation(
	const FCombatAnimationEventDelegate& Delegate)
{
	OnCombatAnimationInterrupted.AddUnique(Delegate);
}

void UAbilityTask_PlayCombatAnimation::Activate()
{
	OnCompleted.AddUniqueDynamic(this, &ThisClass::HandleOriginalAnimationCompleted);
	OnBlendOut.AddUniqueDynamic(this, &ThisClass::HandleOriginalAnimationBlendOut);
	OnCancelled.AddUniqueDynamic(this, &ThisClass::HandleOriginalAnimationCancelled);
	OnInterrupted.AddUniqueDynamic(this, &ThisClass::HandleOriginalAnimationInterrupted);

	PlayAnimation();
}

void UAbilityTask_PlayCombatAnimation::OnDestroy(const bool bInOwnerFinished)
{
	if (IsValid(Ability))
	{
		Ability->OnGameplayAbilityCancelled.Remove(InterruptedHandle);
		if ((bInOwnerFinished && bStopWhenAbilityEnds) || bAlwaysStopMontageOnDestroy)
		{
			StopPlayingAnimationUsingCombatAnimInstance();
		}
	}

	Super::OnDestroy(bInOwnerFinished);
}

UAbilityTask_PlayCombatAnimation* UAbilityTask_PlayCombatAnimation::CreatePlayCombatAnimationProxy(
	UGameplayAbility* OwningAbility, const FName InTaskInstanceName, UAnimMontage* InMontageToPlay, float InRate,
	const FName InStartSection, const bool bInStopWhenAbilityEnds, const float InAnimRootMotionTranslationScale,
	const float InStartTimeSeconds, const bool bInAllowInterruptAfterBlendOut)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(InRate);

	UAbilityTask_PlayCombatAnimation* Task = NewAbilityTask<UAbilityTask_PlayCombatAnimation>(OwningAbility, InTaskInstanceName);
	Task->MontageToPlay = InMontageToPlay;
	Task->Rate = InRate;
	Task->StartSection = InStartSection;
	Task->AnimRootMotionTranslationScale = InAnimRootMotionTranslationScale;
	Task->bStopWhenAbilityEnds = bInStopWhenAbilityEnds;
	Task->bAllowInterruptAfterBlendOut = bInAllowInterruptAfterBlendOut;
	Task->StartTimeSeconds = InStartTimeSeconds;
	return Task;
}

bool UAbilityTask_PlayCombatAnimation::HasSection(const FName SectionName) const
{
	if (IsValid(MontageToPlay))
	{
		const int32 SectionIndex = MontageToPlay->GetSectionIndex(SectionName); 
		return SectionIndex != INDEX_NONE;
	}

	return false;
}

void UAbilityTask_PlayCombatAnimation::MoveToSection(const FName SectionName) const
{
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (IsValid(MontageToPlay) && IsValid(AnimInstance))
	{
		AnimInstance->Montage_JumpToSection(SectionName, MontageToPlay);
	}
}

void UAbilityTask_PlayCombatAnimation::SetPlayRate(const float NewRate) const
{
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (IsValid(MontageToPlay) && IsValid(AnimInstance))
	{
		AnimInstance->Montage_SetPlayRate(MontageToPlay, NewRate);
	}
}

void UAbilityTask_PlayCombatAnimation::ResetPlayRate() const
{
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (IsValid(MontageToPlay) && IsValid(AnimInstance))
	{
		AnimInstance->Montage_SetPlayRate(MontageToPlay, Rate);
	}
}

UAnimInstance* UAbilityTask_PlayCombatAnimation::GetAnimInstance_Implementation() const
{
	if (!IsValid(Ability))
	{
		return nullptr;
	}

	// The combat system should set the correct one in the actor info.
	//
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (ActorInfo->AnimInstance.IsValid() && ActorInfo->AnimInstance->IsValidLowLevelFast())
	{
		return ActorInfo->AnimInstance.Get();
	}

	// If not, then let the combat system try to obtain the anim instance from the combatant.
	//
	UAnimInstance* CombatAnimInstance = UCancerCoreFunctionLibrary::GetCombatAnimInstance(GetAvatarActor());
	if (IsValid(CombatAnimInstance))
	{
		return CombatAnimInstance;
	}
	
	// Otherwise, let the getter method try to figure out the animation instance.
	//
	return Ability->GetActorInfo().GetAnimInstance();
}

void UAbilityTask_PlayCombatAnimation::PlayAnimation()
{
    if (Ability == nullptr)
    {
        return;
    }
	
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!IsValid(ASC))
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWait called on invalid AbilitySystemComponent"));
		return;
	}
	
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWait call with an invalid Animation Instance!"));
		return;
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCombatAnimationReady.Broadcast();
	}
	
    if (!IsValid(MontageToPlay))
    {
        ABILITY_LOG(Warning, TEXT("PlayCombatAnimation called with invalid MontageToPlay"));
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnCancelled.Broadcast();
        }
        return;
    }

    if (StartSection != NAME_None && !HasSection(StartSection))
    {
        ABILITY_LOG(Warning, TEXT("StartSection '%s' not found in montage '%s'. Playing from beginning."), *StartSection.ToString(), *GetNameSafe(MontageToPlay));
        StartSection = NAME_None;
    }

    bool bPlayedMontage = false;
    if (ASC->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection, StartTimeSeconds) > 0.f)
	{
		// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
		if (ShouldBroadcastAbilityTaskDelegates() == false)
		{
			return;
		}

		InterruptedHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UAbilityTask_PlayMontageAndWait::OnGameplayAbilityCancelled);

		BlendingOutDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWait::OnMontageBlendingOut);
		AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

		MontageEndedDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWait::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

		ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
		if (Character && (Character->GetLocalRole() == ROLE_Authority || (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
		{
			Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
		}

		bPlayedMontage = true;
	}

	if (bPlayedMontage)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCombatAnimationStarted.Broadcast();
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWait called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay),*InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast();
		}
	}

	SetWaitingOnAvatar();
}

bool UAbilityTask_PlayCombatAnimation::StopPlayingAnimationUsingCombatAnimInstance()
{
	if (!IsValid(Ability))
	{
		return false;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (ActorInfo == nullptr)
	{
		return false;
	}

	UAnimInstance* AnimInstance = GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		return false;
	}
	
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (IsValid(ASC) && ASC->GetAnimatingAbility() == Ability && AnimInstance->Montage_IsActive(MontageToPlay))
	{
		// Unbind delegates so they don't get called as well.
		FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
		UnbindDelegatesFromMontageInstance(MontageInstance);
		ASC->CurrentMontageStop();
		return true;
	}

	return false;
}

void UAbilityTask_PlayCombatAnimation::UnbindDelegatesFromMontageInstance(FAnimMontageInstance* MontageInstance)
{
	if (MontageInstance)
	{
		MontageInstance->OnMontageBlendingOutStarted.Unbind();
		MontageInstance->OnMontageEnded.Unbind();

		#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 3
		MontageInstance->OnMontageBlendedInEnded.Unbind();
		#endif
	}
}

void UAbilityTask_PlayCombatAnimation::HandleOriginalAnimationCompleted()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCombatAnimationCompleted.Broadcast();
	}
}

void UAbilityTask_PlayCombatAnimation::HandleOriginalAnimationBlendOut()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCombatAnimationBlendOut.Broadcast();
	}	
}

void UAbilityTask_PlayCombatAnimation::HandleOriginalAnimationCancelled()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCombatAnimationCancelled.Broadcast();
	}	
}

void UAbilityTask_PlayCombatAnimation::HandleOriginalAnimationInterrupted()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCombatAnimationInterrupted.Broadcast();
	}	
}
