
#include "AbilitySystem/Abilities/CombatAbility_OpportunityAttack.h"

#include "AbilitySystemComponent.h"
#include "CancerNativeGamePlayTag.h"
#include "ContextualAnimSceneAsset.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/Tasks/AbilityTask_PlayOpportunityAnimation.h"
#include "TargetingSystem/TargetingPreset.h"
#include "TargetingSystem/TargetingSubsystem.h"
#include "AbilityTasks/AbilityTask_PerformTargeting.h"

UCombatAbility_OpportunityAttack::UCombatAbility_OpportunityAttack(const FObjectInitializer& ObjectInitializer)
{
}

bool UCombatAbility_OpportunityAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)
		/*&& IsValid(TargetingPreset)
		&& IsValid(ContextualAnimation)			
		&& !VictimPtr.IsValid()*/;
}

void UCombatAbility_OpportunityAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	TargetingTask = UAbilityTask_PerformTargeting::PerformTargetingRequest(this, TargetingPreset, true);
	if (!IsValid(TargetingTask))
	{
		K2_CancelAbility();
	}

	TargetingTask->OnTargetReady.AddUniqueDynamic(this, &ThisClass::HandleTargetReady);
	TargetingTask->ReadyForActivation();
}

void UCombatAbility_OpportunityAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (HasVictim())
	{
		HandleVictimDeath();
		VictimPtr.Reset();
	}
	
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UCombatAbility_OpportunityAttack::HasVictim() const
{
	return VictimPtr.IsValid() && VictimPtr->IsValidLowLevelFast();
}

AActor* UCombatAbility_OpportunityAttack::GetCurrentVictim() const
{
	if (HasVictim())
	{
		return VictimPtr.Get();
	}

	return nullptr;
}

void UCombatAbility_OpportunityAttack::HandleTargetReady(FTargetingRequestHandle TargetingRequestHandle)
{
	const UTargetingSubsystem* TargetingSubsystem = UTargetingSubsystem::Get(GetWorld());
	check(IsValid(TargetingSubsystem));

	TArray<AActor*> TargetsFound;
	TargetingSubsystem->GetTargetingResultsActors(TargetingRequestHandle, TargetsFound);

	if (TargetsFound.IsValidIndex(0))
	{
		AActor* Target = TargetsFound[0];
		VictimPtr = Target;

		static constexpr bool bExactOnly = false;
		ExecuteOpportunityAttack();
	}
	else if (bHasAbilityFallback && FallbackAbilityTags.IsValid())
	{
		K2_EndAbility();
		UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Ensured();
		AbilitySystemComponent->TryActivateAbilitiesByTag(FallbackAbilityTags);
	}
	else
	{
		K2_CancelAbility();
	}
}

void UCombatAbility_OpportunityAttack::HandleAvatarLeftScene(UContextualAnimSceneActorComponent* SceneActorComponent)
{
	K2_EndAbility();
}

void UCombatAbility_OpportunityAttack::ExecuteOpportunityAttack()
{
	if (HasAuthority(&GetCurrentActivationInfoRef()))
	{
		AActor* Attacker = GetAvatarActorFromActorInfo();
		AActor* Victim = GetCurrentVictim();
		const TMap<FName, AActor*> AdditionalSceneActors = CollectAdditionalSceneActors();

		OpportunityAnimationTask = UAbilityTask_PlayOpportunityAnimation::CreateTask(this, ContextualAnimation, Attacker, Victim);
		OpportunityAnimationTask->OnLeftScene.AddUniqueDynamic(this, &ThisClass::HandleAvatarLeftScene);
		OpportunityAnimationTask->OnSceneFailed.AddUniqueDynamic(this, &ThisClass::HandleAvatarLeftScene);
		OpportunityAnimationTask->SetAdditionalSceneActors(AdditionalSceneActors);
		OpportunityAnimationTask->ReadyForActivation();
	}
	
	if (IsLocallyControlled())
	{
		//执行摄像机
	}
}

TMap<FName, AActor*> UCombatAbility_OpportunityAttack::CollectAdditionalSceneActors_Implementation()
{
	return TMap<FName, AActor*>();
}

void UCombatAbility_OpportunityAttack::HandleVictimDeath()
{
	//取消锁定等
}
