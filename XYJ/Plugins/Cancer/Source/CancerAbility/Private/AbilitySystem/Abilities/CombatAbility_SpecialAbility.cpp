#include "AbilitySystem/Abilities/CombatAbility_SpecialAbility.h"
#include "CancerAbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "GameFramework/CancerCombo.h"

UCombatAbility_SpecialAbility::UCombatAbility_SpecialAbility()
{
	bPlayMontageWhenActivated = false;
}

void UCombatAbility_SpecialAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		K2_EndAbility();
		return;
	}
	auto SpecialAbility = ASC->GetSpecialAbility();
	if (!SpecialAbility)
	{
		K2_EndAbility();
		return;
	}

	CurrentWeaponData = ASC->GetSpecialAbilityComboData(SpecialAbility->ComboTag);
	if (!CurrentWeaponData)
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("Special Ability Combo tag not found[%s]"),
		       *SpecialAbility->ComboTag.ToString());
		K2_EndAbility();
		return;
	}

	if (CurrentWeaponData->bIsStrength)
	{
		WaitReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, /*bTestAlreadyReleased*/true);
		if (IsValid(WaitReleaseTask))
		{
			WaitReleaseTask->OnRelease.AddDynamic(this, &ThisClass::HandleInputReleased);
			WaitReleaseTask->ReadyForActivation();
		}
		StartChargeTime = GetWorld()->GetTimeSeconds();
		bMatchAnimationEnd = false;
		PlayMontageByMontageTag(CurrentWeaponData->PreMontageTag);
	}
	else
	{
		bMatchAnimationEnd = true;
		PlayMontageByMontageTag(CurrentWeaponData->MontageTag);
	}
}

void UCombatAbility_SpecialAbility::HandleInputReleased(float TimeHeld)
{
	if (!CurrentWeaponData)
	{
		K2_EndAbility();
		return;
	}

	float Duration = GetWorld()->GetTimeSeconds() - StartChargeTime;
	bMatchAnimationEnd = true;

	if (Duration >= CurrentWeaponData->DetermineDuration)
	{
		PlayMontageByMontageTag(CurrentWeaponData->PostMontageTag);
	}
	else
	{
		PlayMontageByMontageTag(CurrentWeaponData->MontageTag);
	}
}

void UCombatAbility_SpecialAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(WaitReleaseTask))
	{
		WaitReleaseTask->EndTask();
	}
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	ASC->SetInputTagByAbilityTag(GetAssetTags(), FGameplayTag());
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCombatAbility_SpecialAbility::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);
}

FWeaponData* UCombatAbility_SpecialAbility::GetCurComboData(FGameplayTag ComboTag)
{
	return WeaponDatas.Find(ComboTag);
}
