#pragma once

#include "CoreMinimal.h"
#include "CancerAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/CancerHealthSet.h"
#include "Components/ActorComponent.h"
#include "CancerCombatAttributeComponent.generated.h"


class UCancerEnergySet;
class UCancerStaminaSet;
class UCancerProgressionSet;
class UCancerSoulSet;
class UCancerCombatSet;
class UCancerAttributeSet;
class UCancerAbilitySystemComponent;
class UAbilitySystemComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeathEvent, AActor*, OwningActor, const FGameplayEventData&, EventData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FAttributeChanged, class UCancerCombatAttributeComponent*, Component,
                                              float,
                                              OldValue, float, NewValue, AActor*, Instigator);

UENUM(BlueprintType)
enum class ECancerDeathState : uint8
{
	NotDead = 0 UMETA(DisplayName = "存活"),
	DeathStarted UMETA(DisplayName = "死亡开始"),
	DeathFinished UMETA(DisplayName = "死亡结束")
};

/*
 *战斗资源组件 (复杂角色的属性 及战斗中的资源等)
 */
UCLASS(ClassGroup=(Cancer), meta=(BlueprintSpawnableComponent))
class CANCERABILITY_API UCancerCombatAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCancerCombatAttributeComponent();

	UFUNCTION(BlueprintPure, Category = "Cancer|CombatAttribute")
	static UCancerCombatAttributeComponent* FindCancerCombatAttributeComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UCancerCombatAttributeComponent>() : nullptr);
	}

	UFUNCTION(BlueprintPure)
	UCancerAbilitySystemComponent* GetCancerAbilitySystemComponentFromActor() const;

#pragma region 获取属性

public:
	void InitializeWithAbilitySystem(UCancerAbilitySystemComponent* InASC);

	void UninitializeFromAbilitySystem();

	//属性
	UFUNCTION(BlueprintCallable)
	void RefreshAttribute();

	UFUNCTION(BlueprintCallable, Category = "Cancer|Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Health")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Health")
	float GetHealthNormalized() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Soul")
	float GetSoul() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Soul")
	float GetMaxSoul() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Soul")
	float GetSoulNormalized() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Stamina")
	float GetStamina() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Stamina")
	float GetMaxStamina() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Stamina")
	float GetStaminaNormalized() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Energy")
	float GetEnergy() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Energy")
	float GetMaxEnergy() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Energy")
	float GetEnergyNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Cancer|Progression")
	float GetLevel() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Progression")
	float GetMaxLevel() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Progression")
	float GetExp() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Progression")
	float GetMaxExp() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Combat")
	float GetAttackPower() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Combat")
	float GetPhysicalAttackPower() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Combat")
	float GetDefense() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Combat")
	float GetDamageBonusPct() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Combat")
	float GetDamageReductionPct() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Combat")
	float GetStaggerTime() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Combat")
	float GetStaggerDistance() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Combat")
	float GetImpact() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Combat")
	float GetIronBody() const;

	//常态霸体等级
	UPROPERTY()
	float BaseIronBody = 0.f;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Combat")
	float GetBaseIronBody() const;

	UFUNCTION(BlueprintCallable, Category = "Cancer|Health")
	ECancerDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Cancer|Health",
		Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > ECancerDeathState::NotDead); }

	virtual void StartDeath(const FGameplayEventData& EventData);

	virtual void FinishDeath(const FGameplayEventData& EventData);
	
	UPROPERTY(BlueprintAssignable)
	FOnDeathEvent OnDeathStarted;
	UPROPERTY(BlueprintAssignable)
	FOnDeathEvent OnDeathFinished;

	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnHealthNormalizedChanged;
	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnSoulChanged;
	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnMaxSoulChanged;
	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnSoulNormalizedChanged;
	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnEnergyChanged;
	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnMaxEnergyChanged;
	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnEnergyNormalizedChanged;
	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnStaminaChanged;
	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnMaxStaminaChanged;
	UPROPERTY(BlueprintAssignable)
	FAttributeChanged OnStaminaNormalizedChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cancer|Progression")
	FAttributeChanged OnLevelChanged;
	UPROPERTY(BlueprintAssignable, Category = "Cancer|Combat")
	FAttributeChanged OnAttackPowerChanged;
	UPROPERTY(BlueprintAssignable, Category = "Cancer|Combat")
	FAttributeChanged OnPhysicalAttackPowerChanged;
	UPROPERTY(BlueprintAssignable, Category = "Cancer|Combat")
	FAttributeChanged OnDefenseChanged;
	UPROPERTY(BlueprintAssignable, Category = "Cancer|Combat")
	FAttributeChanged OnDamageBonusPctChanged;
	UPROPERTY(BlueprintAssignable, Category = "Cancer|Combat")
	FAttributeChanged OnDamageReductionPctChanged;
	UPROPERTY(BlueprintAssignable, Category = "Cancer|Combat")
	FAttributeChanged OnStaggerTimeChanged;
	UPROPERTY(BlueprintAssignable, Category = "Cancer|Combat")
	FAttributeChanged OnStaggerDistanceChanged;
	UPROPERTY(BlueprintAssignable, Category = "Cancer|Combat")
	FAttributeChanged OnImpactChanged;
	UPROPERTY(BlueprintAssignable, Category = "Cancer|Combat")
	FAttributeChanged OnIronBodyChanged;


protected:
	UPROPERTY()
	TObjectPtr<UCancerAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<const UCancerHealthSet> HealthSet;
	UPROPERTY()
	TObjectPtr<const UCancerCombatSet> CombatSet;
	UPROPERTY()
	TObjectPtr<const UCancerSoulSet> SoulSet;
	UPROPERTY()
	TObjectPtr<const UCancerProgressionSet> ProgressionSet;
	UPROPERTY()
	TObjectPtr<const UCancerEnergySet> EnergySet;
	UPROPERTY()
	TObjectPtr<const UCancerStaminaSet> StaminaSet;


	UPROPERTY()
	ECancerDeathState DeathState;

	


	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                                 const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
	                                 float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                                    const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
	                                    float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
	                               const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
	                               float NewValue);


	virtual void HandleSoulChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                               const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
	                               float NewValue);
	virtual void HandleMaxSoulChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                                  const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
	                                  float OldValue, float NewValue);
	virtual void HandleOutOfSoul(AActor* DamageInstigator, AActor* DamageCauser,
	                             const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
	                             float NewValue);

	virtual void HandleEnergyChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                                 const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
	                                 float NewValue);
	virtual void HandleMaxEnergyChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                                    const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
	                                    float OldValue, float NewValue);
	virtual void HandleOutOfEnergy(AActor* DamageInstigator, AActor* DamageCauser,
	                               const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
	                               float NewValue);

	virtual void HandleStaminaChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                                  const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
	                                  float OldValue,
	                                  float NewValue);
	virtual void HandleMaxStaminaChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                                     const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
	                                     float OldValue, float NewValue);
	virtual void HandleOutOfStamina(AActor* DamageInstigator, AActor* DamageCauser,
	                                const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
	                                float NewValue);

	void HandleLevelChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec,
	                        float EffectMagnitude, float OldValue, float NewValue);

	void HandleAttackPowerChanged(const struct FOnAttributeChangeData& ChangeData);
	void HandlePhysicalAttackPowerChanged(const struct FOnAttributeChangeData& ChangeData);
	void HandleDefenseChanged(const struct FOnAttributeChangeData& ChangeData);
	void HandleDamageBonusPctChanged(const struct FOnAttributeChangeData& ChangeData);
	void HandleDamageReductionPctChanged(const struct FOnAttributeChangeData& ChangeData);
	void HandleStaggerTimeChanged(const struct FOnAttributeChangeData& ChangeData);
	void HandleStaggerDistanceChanged(const struct FOnAttributeChangeData& ChangeData);
	void HandleImpactChanged(const struct FOnAttributeChangeData& ChangeData);
	void HandleIronBodyChanged(const struct FOnAttributeChangeData& ChangeData);

#pragma endregion
};
