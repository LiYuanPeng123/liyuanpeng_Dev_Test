#include "AbilitySystem/Abilities/CombatAbility_LoopAbility.h"

#include "CancerAbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

UCombatAbility_LoopAbility::UCombatAbility_LoopAbility()
{
	ActivationPolicy = ECancerAbilityActivationPolicy::None;
}

void UCombatAbility_LoopAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!bInputRelease)
		return;
	WaitReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	if (IsValid(WaitReleaseTask))
	{
		WaitReleaseTask->OnRelease.AddDynamic(this, &ThisClass::HandleInputReleased);
		WaitReleaseTask->ReadyForActivation();
	}
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	if (auto MontageToPlay = ASC->GetAbilityMontage(MontageTag))
	{
		SetupAndPlayAnimation(MontageToPlay);
		bInputRelease = false;
	}
	else
	{
		K2_EndAbility();
	}
}

void UCombatAbility_LoopAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            bool bReplicateEndAbility, bool bWasCancelled)
{
	FinishLatentTask(WaitReleaseTask);
	bInputRelease = true;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCombatAbility_LoopAbility::HandleInputReleased(float TimeHeld)
{
    if (HasSection(TEXT("End")))
    {
        MoveToSection(TEXT("End"));
    }
    else
    {
        K2_EndAbility();
    }
}
