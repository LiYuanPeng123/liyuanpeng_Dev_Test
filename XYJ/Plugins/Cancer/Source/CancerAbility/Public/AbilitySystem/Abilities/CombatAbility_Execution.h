#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "CombatAbility_Execution.generated.h"

class UAbilityTask_PlayExecutionSequence;
class UAbilityTask_PerformTargeting;
class ULevelSequence;
class UTargetingPreset;
struct FTargetingRequestHandle;
/**
 * 处决技能
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_Execution : public UCancerGameplayAbility
{
	GENERATED_BODY()

public:
	UCombatAbility_Execution(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags = nullptr,
	                                const FGameplayTagContainer* TargetTags = nullptr,
	                                FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	UFUNCTION(BlueprintPure, Category = "Combat|Abilities")
	bool HasVictim() const;

	UFUNCTION(BlueprintPure, Category = "Combat|Abilities")
	AActor* GetCurrentVictim() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Execution")
	ULevelSequence* ExecutionSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Execution")
	UTargetingPreset* TargetingPreset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Execution")
	FName AttackerBindingTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Execution")
	FName VictimBindingTag;

	UFUNCTION(BlueprintCallable, Category = "Combat|Abilities")
	void ExecuteExecution();

	UFUNCTION(BlueprintNativeEvent, Category = "Execution")
	TMap<FName, AActor*> CollectAdditionalSequenceBindings();

	UFUNCTION(BlueprintCallable, Category = "Combat|Abilities")
	virtual void HandleVictimDeath();

	UFUNCTION()
	void HandleTargetReady(FTargetingRequestHandle TargetingRequestHandle);
	UFUNCTION()
	void HandleSequenceFinished();

private:
	TWeakObjectPtr<AActor> VictimPtr;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PerformTargeting> TargetingTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayExecutionSequence> SequenceTask;
};
