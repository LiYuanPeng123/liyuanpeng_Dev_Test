// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CancerCombatAnimNotifyState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UCancerCombatAnimNotifyState::UCancerCombatAnimNotifyState()
{
	bExpired = false;
	PlayRate = 1.f;
	ElapsedTime = 0.f;
	NotifyDuration = 0.f;
	EffectiveDuration = 0.f;
	
#if WITH_EDITORONLY_DATA
	// Ninja Bear Green! :D
	NotifyColor = FColor(211, 221, 197);
#endif
}

void UCancerCombatAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	bExpired = false;
	ElapsedTime = 0.f;
	NotifyDuration = TotalDuration;
	PlayRate = GetActualPlayRate(MeshComp->GetOwner(), Animation);
	EffectiveDuration = NotifyDuration / PlayRate;
}

void UCancerCombatAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	ElapsedTime += FrameDeltaTime;
	if (ElapsedTime >= EffectiveDuration && !bExpired)
	{
		HandleTimeExpired(MeshComp, Animation);
		bExpired = true;
	}
}

void UCancerCombatAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (!bExpired)
	{
		HandleTimeExpired(MeshComp, Animation);
		bExpired = true;
	}
}

UAbilitySystemComponent* UCancerCombatAnimNotifyState::GetAbilitySystemComponent(const AActor* Owner)
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
}

float UCancerCombatAnimNotifyState::GetActualPlayRate(AActor* Owner, const UAnimSequenceBase* Animation) const
{
	if (!IsValid(Owner) || !IsValid(Animation))
	{
		return 1.f;
	}

	const float AnimationPlayRate = Animation->RateScale;
	
	const UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent(Owner);
	if (!IsValid(AbilitySystemComponent) || AbilitySystemComponent->GetCurrentMontage() != Animation)
	{
		return AnimationPlayRate;
	}
	
	const TWeakObjectPtr<UAnimInstance> AnimInstancePtr = AbilitySystemComponent->AbilityActorInfo->AnimInstance;
	if (!AnimInstancePtr.IsValid() || !AnimInstancePtr->IsValidLowLevelFast())
	{
		return AnimationPlayRate;
	}
	
	const UAnimMontage* CurrentMontage = AbilitySystemComponent->GetCurrentMontage();
	return AnimInstancePtr->Montage_GetEffectivePlayRate(CurrentMontage);
}

void UCancerCombatAnimNotifyState::HandleTimeExpired(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
}
