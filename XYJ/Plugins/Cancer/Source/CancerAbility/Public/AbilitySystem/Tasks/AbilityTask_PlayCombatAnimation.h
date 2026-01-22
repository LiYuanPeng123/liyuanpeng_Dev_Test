
#pragma once

#include "CoreMinimal.h"
#include "CancerCombatDelegates.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/Interfaces/CancerCombatAnimationTaskInterface.h"
#include "AbilityTask_PlayCombatAnimation.generated.h"

/**
 * 
 */
UCLASS()
class CANCERABILITY_API UAbilityTask_PlayCombatAnimation : public UAbilityTask_PlayMontageAndWait
	,public ICancerCombatAnimationTaskInterface
{
	GENERATED_BODY()

	public:

	UPROPERTY(BlueprintAssignable)
	FCombatAnimationEventMCDelegate OnCombatAnimationReady;
	
	UPROPERTY(BlueprintAssignable)
	FCombatAnimationEventMCDelegate OnCombatAnimationStarted;
	
	UPROPERTY(BlueprintAssignable)
	FCombatAnimationEventMCDelegate OnCombatAnimationCompleted;

	UPROPERTY(BlueprintAssignable)
	FCombatAnimationEventMCDelegate OnCombatAnimationBlendOut;

	UPROPERTY(BlueprintAssignable)
	FCombatAnimationEventMCDelegate OnCombatAnimationCancelled;	
	
	UPROPERTY(BlueprintAssignable)
	FCombatAnimationEventMCDelegate OnCombatAnimationInterrupted;
	
	// -- Begin Combat Animation Task implementation
	virtual void BindToAnimationReady_Implementation(const FCombatAnimationEventDelegate& Delegate) override;
	virtual void BindToAnimationStarted_Implementation(const FCombatAnimationEventDelegate& Delegate) override;
	virtual void BindToAnimationCompleted_Implementation(const FCombatAnimationEventDelegate& Delegate) override;
	virtual void BindToAnimationBlendOut_Implementation(const FCombatAnimationEventDelegate& Delegate) override;
	virtual void BindToAnimationCancelled_Implementation(const FCombatAnimationEventDelegate& Delegate) override;
	virtual void BindToAnimationInterrupted_Implementation(const FCombatAnimationEventDelegate& Delegate) override;
	// -- End Combat Animation Task implementation

	// -- Begin Ability Task implementation
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	// -- End Ability Task implementation


	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName="PlayCombatMontageAndWait", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_PlayCombatAnimation* CreatePlayCombatAnimationProxy(UGameplayAbility* OwningAbility,
		FName InTaskInstanceName, UAnimMontage* InMontageToPlay, float InRate = 1.f, FName InStartSection = NAME_None,
		bool bInStopWhenAbilityEnds = true, float InAnimRootMotionTranslationScale = 1.f, float InStartTimeSeconds = 0.f,
		bool bInAllowInterruptAfterBlendOut = false);


	UFUNCTION(BlueprintCallable, Category = "Ninja Combat|Tasks")
	virtual bool HasSection(FName SectionName) const;

	UFUNCTION(BlueprintCallable, Category = "Ninja Combat|Tasks")
	virtual void MoveToSection(FName SectionName) const;

	
	UFUNCTION(BlueprintCallable, Category = "Ninja Combat|Tasks")
	virtual void SetPlayRate(float NewRate) const;


	UFUNCTION(BlueprintCallable, Category = "Ninja Combat|Tasks")
	virtual void ResetPlayRate() const;
	
protected:


	bool bAlwaysStopMontageOnDestroy = true;


	UFUNCTION(BlueprintNativeEvent, Category = "Combat Animation")
	UAnimInstance* GetAnimInstance() const;

	virtual void PlayAnimation();

	virtual bool StopPlayingAnimationUsingCombatAnimInstance();

	virtual void UnbindDelegatesFromMontageInstance(FAnimMontageInstance* MontageInstance);
	
	UFUNCTION()
	virtual void HandleOriginalAnimationCompleted();

	UFUNCTION()
	virtual void HandleOriginalAnimationBlendOut();
	
	UFUNCTION()
	virtual void HandleOriginalAnimationCancelled();	

	UFUNCTION()
	virtual void HandleOriginalAnimationInterrupted();
};
