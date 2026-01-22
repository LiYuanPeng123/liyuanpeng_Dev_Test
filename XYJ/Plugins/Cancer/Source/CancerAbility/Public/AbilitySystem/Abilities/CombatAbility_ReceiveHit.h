#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "GameFramework/CancerDamageType.h"
#include "CombatAbility_ReceiveHit.generated.h"

class UCancerStaggerComponent;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiveHit, FGameplayEventData, Payload);

/**
 * 受击GA：根据 UCancerDamageType 与受击者状态判断是否结算伤害
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_ReceiveHit : public UCancerGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnReceiveHit ReceiveDamage;
	UPROPERTY(BlueprintAssignable)
	FOnReceiveHit ReceiveBlock;
	UPROPERTY(BlueprintAssignable)
	FOnReceiveHit ReceivePerfectBlock;
	UPROPERTY(BlueprintAssignable)
	FOnReceiveHit ReceiveInvulnerable;

	UCombatAbility_ReceiveHit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// -- Begin Gameplay Ability implementation
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;


	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;

	UFUNCTION(BlueprintNativeEvent)
	void HandleReceiveDamage(const FGameplayEventData& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandleReceiveBlock(const FGameplayEventData& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandleReceivePerfectBlock(const FGameplayEventData& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandleReceiveInvulnerable(const FGameplayEventData& Payload);

	void SendDamageFeedback(const UCancerDamageType* DamageInfo,const FGameplayTagContainer& FeedBackInfo);
	
	void HandleOutofHealth(AActor* DamageInstigator, 
		AActor* DamageCauser,
		const FGameplayEffectSpec* DamageEffectSpec,
		float DamageMagnitude, 
		float OldValue, 
		float NewValue);
	
	// 角色是否因为本次伤害死亡
	bool bCharacterDead {false};
	
	// 角色是否因为本次伤害气绝
	bool bCharacterNoSoul {false};

protected:
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> DamageTask;
	
private:
	FDelegateHandle CharacterDeadDelegateHandle;
	
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitCharacterDeathEventTask;
	
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitCharacterNoSoulEventTask;
};
