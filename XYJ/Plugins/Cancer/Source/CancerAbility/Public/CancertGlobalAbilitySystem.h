#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameFramework/CancerDamageType.h"
#include "UIExtensionSystem.h"
#include "CancertGlobalAbilitySystem.generated.h"

struct FCancerComboInfo;
class UGameplayAbility;
class UGameplayEffect;
class UCancerAbilitySystemComponent;

USTRUCT()
struct FGlobalAppliedAbilityList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<UCancerAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;

	void AddToASC(TSubclassOf<UGameplayAbility> Ability, UCancerAbilitySystemComponent* ASC);
	void RemoveFromASC(UCancerAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

USTRUCT()
struct FGlobalAppliedEffectList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<UCancerAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;

	void AddToASC(TSubclassOf<UGameplayEffect> Effect, UCancerAbilitySystemComponent* ASC);
	void RemoveFromASC(UCancerAbilitySystemComponent* ASC);
	void RemoveFromAll();
};


UCLASS()
class CANCERABILITY_API UCancerGlobalAbilitySystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UCancerGlobalAbilitySystem();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Cancer")
	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Cancer")
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Cancer")
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Cancer")
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

	/** Register an ASC with global system and apply any active global effects/abilities. */
	void RegisterASC(UCancerAbilitySystemComponent* ASC);

	/** Removes an ASC from the global system, along with any active global effects/abilities. */
	void UnregisterASC(UCancerAbilitySystemComponent* ASC);

	// -- Begin Subsystem implementation
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// -- End Subsystem implementation

	UFUNCTION(BlueprintCallable, Category="Cancer|CombatSlomo")
	void K2_CombatSlomo(AActor* Attacker, AActor*
	                    Victim, const
	                    FCancerComboInfo& ComboInfo);
	
	// 通过名字找到Tag  Tag的名字配置在项目设置 -- GamplayTag 里的表格上
	static FGameplayTag GetTagByName(const FName TagName);

protected:
	//~ Begin FTickableGameObject Interface
	virtual bool IsTickable() const override { return true; }
	virtual bool IsTickableInEditor() const override { return true; }
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	//~ End FTickableGameObject Interface

	virtual void InitializeStream();

	UFUNCTION(BlueprintPure, Category = "Ninja Combat")
	float GetRandomFloatInRange(float Min, float Max) const;

	UFUNCTION()
	void HandleSlomo(AActor* Attacker, AActor* Victim, const FCancerComboInfo& ComboInfo);

private:
	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<AActor>, float> BossNextAllowedShowTime;

	FTimerHandle TimeHandle;

	FTimerDelegate TimerDelegate;

	TWeakObjectPtr<AActor> CachedAttacker;

	TWeakObjectPtr<AActor> CachedVictim;

	float Elapsed{0.f};
	FCancerComboInfo CurrentComboInfo;

	FRandomStream RandomStream;
	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

	UPROPERTY()
	TArray<TObjectPtr<UCancerAbilitySystemComponent>> RegisteredASCs;
	
	static TMap<FName,FGameplayTag> GlobalTagNameMap;
};
