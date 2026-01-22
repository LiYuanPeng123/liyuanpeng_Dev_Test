#include "AbilitySystem/Abilities/CombatAbility_Execution.h"
#include "AbilitySystem/Tasks/AbilityTask_PlayExecutionSequence.h"
#include "AbilityTasks/AbilityTask_PerformTargeting.h"
#include "TargetingSystem/TargetingPreset.h"
#include "TargetingSystem/TargetingSubsystem.h"
#include "AbilitySystemComponent.h"

UCombatAbility_Execution::UCombatAbility_Execution(const FObjectInitializer& ObjectInitializer)
{
}

bool UCombatAbility_Execution::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayTagContainer* SourceTags,
                                                  const FGameplayTagContainer* TargetTags,
                                                  FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UCombatAbility_Execution::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
	if (IsValid(TargetingPreset))
	{
		TargetingTask = UAbilityTask_PerformTargeting::PerformTargetingRequest(this, TargetingPreset, true);
		if (!IsValid(TargetingTask))
		{
			K2_CancelAbility();
			return;
		}
		TargetingTask->OnTargetReady.AddUniqueDynamic(this, &ThisClass::HandleTargetReady);
		TargetingTask->ReadyForActivation();
		return;
	}

	if (TriggerEventData)
	{
		AActor* Attacker = GetAvatarActorFromActorInfo();
		AActor* Candidate = const_cast<AActor*>(TriggerEventData->Target.Get());
		if (Candidate == Attacker)
		{
			Candidate = const_cast<AActor*>(TriggerEventData->Instigator.Get());
		}
		if (IsValid(Candidate) && Candidate != Attacker)
		{
			VictimPtr = Candidate;
			ExecuteExecution();
			return;
		}
	}

	K2_CancelAbility();
}

void UCombatAbility_Execution::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          bool bReplicateEndAbility, bool bWasCancelled)
{
	if (HasVictim())
	{
		HandleVictimDeath();
		VictimPtr.Reset();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UCombatAbility_Execution::HasVictim() const
{
	return VictimPtr.IsValid() && VictimPtr->IsValidLowLevelFast();
}

AActor* UCombatAbility_Execution::GetCurrentVictim() const
{
	if (HasVictim())
	{
		return VictimPtr.Get();
	}
	return nullptr;
}

void UCombatAbility_Execution::HandleTargetReady(FTargetingRequestHandle TargetingRequestHandle)
{
	const UTargetingSubsystem* TargetingSubsystem = UTargetingSubsystem::Get(GetWorld());
	if (!IsValid(TargetingSubsystem))
	{
		K2_CancelAbility();
		return;
	}

	TArray<AActor*> TargetsFound;
	TargetingSubsystem->GetTargetingResultsActors(TargetingRequestHandle, TargetsFound);

	if (TargetsFound.IsValidIndex(0))
	{
		AActor* Target = TargetsFound[0];
		VictimPtr = Target;
		ExecuteExecution();
	}
	else
	{
		K2_CancelAbility();
	}
}

void UCombatAbility_Execution::HandleSequenceFinished()
{
	K2_EndAbility();
}

void UCombatAbility_Execution::ExecuteExecution()
{
	if (HasAuthority(&GetCurrentActivationInfoRef()))
	{
		AActor* Attacker = GetAvatarActorFromActorInfo();
		AActor* Victim = GetCurrentVictim();

		TMap<FName, AActor*> Bindings = CollectAdditionalSequenceBindings();
		if (AttackerBindingTag != NAME_None && IsValid(Attacker))
		{
			Bindings.Add(AttackerBindingTag, Attacker);
		}
		if (VictimBindingTag != NAME_None && IsValid(Victim))
		{
			Bindings.Add(VictimBindingTag, Victim);
		}

		AActor* Origin = IsValid(Victim) ? Victim : Attacker;

		SequenceTask = UAbilityTask_PlayExecutionSequence::CreateTask(this, ExecutionSequence, Origin, Bindings);
		SequenceTask->OnSequenceFinished.AddUniqueDynamic(this, &ThisClass::HandleSequenceFinished);
		SequenceTask->OnSequenceFailed.AddUniqueDynamic(this, &ThisClass::HandleSequenceFinished);
		SequenceTask->ReadyForActivation();
	}

	if (IsLocallyControlled())
	{
	}
}

TMap<FName, AActor*> UCombatAbility_Execution::CollectAdditionalSequenceBindings_Implementation()
{
	return TMap<FName, AActor*>();
}

void UCombatAbility_Execution::HandleVictimDeath()
{
}
