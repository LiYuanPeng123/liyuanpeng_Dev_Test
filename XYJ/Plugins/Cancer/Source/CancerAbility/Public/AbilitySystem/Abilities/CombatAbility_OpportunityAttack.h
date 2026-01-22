#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "CombatAbility_OpportunityAttack.generated.h"

class UAbilityTask_PlayOpportunityAnimation;
class UAbilityTask_PerformTargeting;
class UAbilityTask_NetworkSyncPoint;
class UContextualAnimSceneActorComponent;
struct FTargetingRequestHandle;
class UContextualAnimSceneAsset;
class UTargetingPreset;
/**
 * 处决
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_OpportunityAttack : public UCancerGameplayAbility
{
	GENERATED_BODY()

public:
	UCombatAbility_OpportunityAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Opportunity Attack")
	TObjectPtr<UTargetingPreset> TargetingPreset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Opportunity Attack")
	UContextualAnimSceneAsset* ContextualAnimation;

	UFUNCTION()
	void HandleTargetReady(FTargetingRequestHandle TargetingRequestHandle);
	UFUNCTION()
	void HandleAvatarLeftScene(UContextualAnimSceneActorComponent* SceneActorComponent);

	UFUNCTION(BlueprintCallable, Category = "Combat|Abilities")
	void ExecuteOpportunityAttack();

	UFUNCTION(BlueprintNativeEvent, Category = "Opportunity Attack")
	TMap<FName, AActor*> CollectAdditionalSceneActors();

	UFUNCTION(BlueprintCallable, Category = "Combat|Abilities")
	virtual void HandleVictimDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Opportunity Attack",
		meta = (InlineEditConditionToggle))
	bool bHasAbilityFallback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Opportunity Attack",
		meta = (EditCondition = "bHasAbilityFallback"))
	FGameplayTagContainer FallbackAbilityTags;

private:
	TWeakObjectPtr<AActor> VictimPtr;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PerformTargeting> TargetingTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayOpportunityAnimation> OpportunityAnimationTask;
};
