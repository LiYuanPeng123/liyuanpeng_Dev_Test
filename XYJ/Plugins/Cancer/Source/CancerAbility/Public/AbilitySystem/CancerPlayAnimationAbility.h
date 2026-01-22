
#pragma once

#include "CoreMinimal.h"
#include "CancerGameplayAbility.h"
#include "CancerPlayAnimationAbility.generated.h"

/**
 *  播放蒙太奇
 */
UCLASS()
class CANCERABILITY_API UCancerPlayAnimationAbility : public UCancerGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "Combat Ability")
	void SetupAndPlayAnimation(UAnimMontage* Montage, FName SectionName = NAME_None, float PlayRate = 1.f,
	                           float RootMotionScale = 1.f);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	void BeforeAnimationReady();
	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	void OnAnimationReady();
	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	void OnAnimationStarted();
	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	void OnAnimationFinished();
	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	void OnAnimationCancelled();

	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	bool HasSection(FName SectionName) const;
	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	void MoveToSection(FName SectionName) const;
	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	void SetMontagePlayRate(float PlayRate);
	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	void ResetMontagePlayRate();

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	UAbilityTask* InitializeAnimationTask(UAnimMontage* Montage, FName SectionName, float PlayRate,
	                                      float RootMotionScale);

	UFUNCTION(BlueprintCallable, Category = "Combat Anim Ability")
	void PlayMontageByMontageTag(FGameplayTag TargetAbilityMontageTag);

	UFUNCTION(BlueprintNativeEvent,BlueprintPure, Category = "Combat Anim Ability")
	UAnimMontage* GetMontage();

protected:
	UPROPERTY()
	UAbilityTask* AnimationTask;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat Ability|Animation", DisplayName="DefaultMontage")
	FGameplayTag DefaultMontageTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Animation")
	bool bPlayMontageWhenActivated = false;
	
};
