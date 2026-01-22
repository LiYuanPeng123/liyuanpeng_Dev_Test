#include "AbilitySystem/Abilities/CombatAbility_Weakness.h"

#include "CancerAbilityFunctionLibrary.h"
#include "CancerAbilitySystemComponent.h"
#include "CancerNativeGamePlayTag.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/Effects/CancerEffect_Soul.h"
#include "Components/CancerCombatAttributeComponent.h"


UE_DISABLE_OPTIMIZATION

UCombatAbility_Weakness::UCombatAbility_Weakness()
{
	// Old Way
	// FAbilityTriggerData TriggerData;
	// TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	// TriggerData.TriggerTag = Tag_Combat_Event_Weak;
	// AbilityTriggers.Add(TriggerData);
}

void UCombatAbility_Weakness::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// if (TriggerEventData)
	// 	HandleEventReceived(*TriggerEventData);
	// else
	// {
	// 	K2_EndAbility();
	// }
}

void UCombatAbility_Weakness::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         bool bReplicateEndAbility, bool bWasCancelled)
{
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	UCancerCombatAttributeComponent* CombatAttributeComponent = GetCancerCombatAttributeComponentFromActorInfo();
	ASC->RemoveLooseGameplayTag(Tag_Combat_State_Weak);

	check(ASC);
	check(CombatAttributeComponent);
	UCancerAbilityFunctionLibrary::K2_ApplySoulRegen(GetOwningActorFromActorInfo(), CombatAttributeComponent
	                                                 ->GetMaxSoul(), ASC->GetSoulData().SoulReplyTime,
	                                                 UCancerEffect_Soul::StaticClass());
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCombatAbility_Weakness::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);
	if (Payload.EventTag == Tag_Combat_Event_AbilityTrigger_Weak)
	{
		auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
		check(ASC);
		float WeakTime = ASC->GetSoulData().WeakTime;

		//是否被弹反进入虚弱
		//Payload.OptionalObject.Get()

		SetupAndPlayAnimation(GetMontage());
		ASC->AddLooseGameplayTag(Tag_Combat_State_Weak);
		WaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, WeakTime);
		if (WaitDelay)
		{
			WaitDelay->OnFinish.AddDynamic(this, &ThisClass::HandleWeakFinish);
			WaitDelay->ReadyForActivation();
		}
	}
}

void UCombatAbility_Weakness::HandleWeakFinish()
{
	if (HasSection("F3"))
	{
		MoveToSection("F3");
		return;
	}
	K2_EndAbility();
}

UE_ENABLE_OPTIMIZATION
