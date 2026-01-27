#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "CancerCombatAnimationAbility.generated.h"

class UCancerCombatAnimationProvider;

UCLASS()
class CANCERABILITY_API UCancerCombatAnimationAbility : public UCancerGameplayAbility
{
	GENERATED_BODY()

public:
	static FName AnimationProviderName;

	UCancerCombatAnimationAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	void PlayAnimationMontage();

	UFUNCTION(BlueprintPure, Category = "Combat Ability")
	virtual bool HasSection(FName SectionName) const;

	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	virtual void MoveToSection(FName SectionName) const;

	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	virtual void SetMontagePlayRate(float PlayRate);

	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	virtual void ResetMontagePlayRate();

public:
	// 获取当前用于播放的动画任务（若存在）。
	UFUNCTION(BlueprintCallable, Category = "Combat Ability|Animation")
	UAbilityTask* GetAnimationTask() const { return AnimationTask; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Animation", Instanced, NoClear,
		meta = (ShowInnerProperties, EditConditionHides))
	TObjectPtr<UCancerCombatAnimationProvider> AnimationProvider;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Animation")
	bool bPlayMontageWhenActivated;


	// -- Begin Gameplay Ability implementation
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;
	// -- End Gameplay Ability implementation

	UPROPERTY()
	TObjectPtr<UAbilityTask> AnimationTask;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	UAbilityTask* InitializeAnimationTask();

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	void SetupAndPlayAnimation();

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	void OnAnimationReady();

	virtual void OnAnimationReady_Implementation()
	{
	}

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	void OnAnimationStarted();

	virtual void OnAnimationStarted_Implementation()
	{
	}

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	void OnAnimationFinished();
	virtual void OnAnimationFinished_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	void OnAnimationCancelled();
	virtual void OnAnimationCancelled_Implementation();
};
