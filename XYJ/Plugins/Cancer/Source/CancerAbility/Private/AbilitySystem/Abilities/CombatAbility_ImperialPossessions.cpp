#include "AbilitySystem/Abilities/CombatAbility_ImperialPossessions.h"
#include "CancerNativeGamePlayTag.h"
#include "AbilitySystem/Tasks/AbilityTask_TargetingTick.h"
#include "TargetingSystem/TargetingSubsystem.h"


bool UCombatAbility_ImperialPossessions::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                            const FGameplayAbilityActorInfo* ActorInfo,
                                                            const FGameplayTagContainer* SourceTags,
                                                            const FGameplayTagContainer* TargetTags,
                                                            FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UCombatAbility_ImperialPossessions::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                         const FGameplayAbilityActorInfo* ActorInfo,
                                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCombatAbility_ImperialPossessions::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                                    bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	FinishLatentTask(TargetingTick);
}

void UCombatAbility_ImperialPossessions::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);
	if (Payload.EventTag == Tag_Combat_Event_TargetingPreset_Start)
	{
		TargetingTick = InitializeTargetingPresetTask(TargetingPreset);
		TargetingTick->ReadyForActivation();
		TargetingTick->OnTargetUpdated.AddDynamic(this, &ThisClass::HandleTargetReady);
	}
	else if (Payload.EventTag == Tag_Combat_Event_TargetingPreset_Stop)
	{
		FinishLatentTask(TargetingTick);
	}
}

UAbilityTask_TargetingTick* UCombatAbility_ImperialPossessions::InitializeTargetingPresetTask(
	UTargetingPreset* InTargetingPreset)
{
	return UAbilityTask_TargetingTick::TargetingPresetTick(this, InTargetingPreset);
}

void UCombatAbility_ImperialPossessions::HandleTargetReady(const FTargetingRequestHandle Payload)
{
	const UTargetingSubsystem* TargetingSubsystem = UTargetingSubsystem::Get(GetWorld());
	check(IsValid(TargetingSubsystem));

	TArray<AActor*> TargetsFound;
	TargetingSubsystem->GetTargetingResultsActors(Payload, TargetsFound);

	for (AActor* Actor : TargetsFound)
	{
		UE_LOG(LogAbilitySystemComponent, Warning, TEXT("%s"), *Actor->GetName());
	}
}
