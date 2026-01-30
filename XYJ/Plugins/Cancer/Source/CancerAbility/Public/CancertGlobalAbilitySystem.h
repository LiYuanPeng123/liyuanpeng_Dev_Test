#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameFramework/CancerDamageType.h"
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

USTRUCT()
struct FAppliedSlomoHandle
{
	GENERATED_BODY()
	FAppliedSlomoHandle()
	{
		Attacker = nullptr;
		Victim = nullptr;
	};
	explicit FAppliedSlomoHandle(const FCancerComboInfo& Info,
	                             AActor* InAttacker = nullptr, AActor* InVictim = nullptr)
	{
		Guid = FGuid::NewGuid();
		ComboInfo = Info;
		Attacker = InAttacker;
		Victim = InVictim;
	}

	bool operator==(const FAppliedSlomoHandle& Other) const
	{
		return Guid == Other.Guid;
	}

	bool operator!=(const FAppliedSlomoHandle& Other) const
	{
		return Guid != Other.Guid;
	}

	FGuid GetGuid() const
	{
		return Guid;
	}

	bool IsValid() const
	{
		return Guid.IsValid();
	}

	UPROPERTY()
	FCancerComboInfo ComboInfo;
	UPROPERTY()
	AActor* Attacker;
	UPROPERTY()
	AActor* Victim;

	UPROPERTY()
	float ElapsedTime = 0.f;

private:
	UPROPERTY()
	FGuid Guid;
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
	// 通过名字找到Tag  Tag的名字配置在项目设置 -- GamplayTag 里的表格上
	static FGameplayTag GetTagByName(const FName TagName);


#pragma region  顿帧
	
public:
	// 更新顿帧逻辑
	void UpdateSlomo(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	FGuid RegisterSlomo(AActor* Attacker, AActor* Victim, const FCancerComboInfo& ComboInfo);

protected:
	UFUNCTION()
	void RemoveSlomo(FGuid Guid);

	UPROPERTY()
	TMap<FGuid, FAppliedSlomoHandle> AppliedSlomo;

	UPROPERTY()
	TMap<FGuid, FTimerHandle> SlomoTimerHandles;
#pragma endregion
	

protected:
	//~ Begin FTickableGameObject Interface
	virtual bool IsTickable() const override { return true; }
	virtual bool IsTickableInEditor() const override { return true; }
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	//~ End FTickableGameObject Interface

	virtual void InitializeStream();

	UFUNCTION(BlueprintPure, Category = " Combat")
	float GetRandomFloatInRange(float Min, float Max) const;




private:
	FRandomStream RandomStream;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

	UPROPERTY()
	TArray<TObjectPtr<UCancerAbilitySystemComponent>> RegisteredASCs;

	static TMap<FName, FGameplayTag> GlobalTagNameMap;
};
