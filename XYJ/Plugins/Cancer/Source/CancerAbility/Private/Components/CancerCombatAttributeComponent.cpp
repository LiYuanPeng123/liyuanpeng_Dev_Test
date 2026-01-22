#include "Components/CancerCombatAttributeComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerAbilitySystemComponent.h"
#include "CancerAssetManager.h"
#include "CancerCoreSetting.h"
#include "AbilitySystem/Attributes/CancerCombatSet.h"
#include "AbilitySystem/Attributes/CancerEnergySet.h"
#include "AbilitySystem/Attributes/CancerHealthSet.h"
#include "AbilitySystem/Attributes/CancerProgressionSet.h"
#include "AbilitySystem/Attributes/CancerSoulSet.h"
#include "AbilitySystem/Attributes/CancerStaminaSet.h"
#include "Data/CancerCombatData.h"

class UCancerProgressionSet;


DEFINE_LOG_CATEGORY_STATIC(LogAttributeComponents, Log, All);

UCancerCombatAttributeComponent::UCancerCombatAttributeComponent()
{
	DeathState = ECancerDeathState::NotDead;
}


UCancerAbilitySystemComponent* UCancerCombatAttributeComponent::GetCancerAbilitySystemComponentFromActor() const
{
	return AbilitySystemComponent;
}


void UCancerCombatAttributeComponent::InitializeWithAbilitySystem(UCancerAbilitySystemComponent* InASC)
{
	ensure(InASC);
	AbilitySystemComponent = InASC;
	HealthSet = AbilitySystemComponent->GetSet<UCancerHealthSet>();
	CombatSet = AbilitySystemComponent->GetSet<UCancerCombatSet>();
	SoulSet = AbilitySystemComponent->GetSet<UCancerSoulSet>();
	ProgressionSet = AbilitySystemComponent->GetSet<UCancerProgressionSet>();
	StaminaSet = AbilitySystemComponent->GetSet<UCancerStaminaSet>();
	EnergySet = AbilitySystemComponent->GetSet<UCancerEnergySet>();
	ensure(HealthSet);
	ensure(CombatSet);
	ensure(SoulSet);
	ensure(ProgressionSet);
	ensure(StaminaSet);
	ensure(EnergySet);
	if (HealthSet)
	{
		HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
		HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
		HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);
		AbilitySystemComponent->SetNumericAttributeBase(UCancerHealthSet::GetHealthAttribute(),
		                                                HealthSet->GetMaxHealth());
		OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
		OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	}
	if (ProgressionSet)
	{
		ProgressionSet->OnLevelChanged.AddUObject(this, &ThisClass::HandleLevelChanged);
		OnLevelChanged.Broadcast(this, ProgressionSet->GetLevel(), ProgressionSet->GetLevel(), nullptr);
	}
	if (SoulSet)
	{
		SoulSet->OnSoulChanged.AddUObject(this, &ThisClass::HandleSoulChanged);
		SoulSet->OnMaxSoulChanged.AddUObject(this, &ThisClass::HandleMaxSoulChanged);
		SoulSet->OnOutOfSoul.AddUObject(this, &ThisClass::HandleOutOfSoul);
	}
	if (StaminaSet)
	{
		StaminaSet->OnStaminaChanged.AddUObject(this, &ThisClass::HandleStaminaChanged);
		StaminaSet->OnMaxStaminaChanged.AddUObject(this, &ThisClass::HandleMaxStaminaChanged);
		StaminaSet->OnOutOfStamina.AddUObject(this, &ThisClass::HandleOutOfStamina);
	}
	if (EnergySet)
	{
		EnergySet->OnEnergyChanged.AddUObject(this, &ThisClass::HandleEnergyChanged);
		EnergySet->OnMaxEnergyChanged.AddUObject(this, &ThisClass::HandleMaxEnergyChanged);
		EnergySet->OnOutOfEnergy.AddUObject(this, &ThisClass::HandleOutOfEnergy);
	}
	if (CombatSet)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetAttackPowerAttribute())
		                      .AddUObject(this, &ThisClass::HandleAttackPowerChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			                      UCancerCombatSet::GetPhysicalAttackPowerAttribute())
		                      .AddUObject(this, &ThisClass::HandlePhysicalAttackPowerChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetDefenseAttribute())
		                      .AddUObject(this, &ThisClass::HandleDefenseChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetDamageBonusPctAttribute())
		                      .AddUObject(this, &ThisClass::HandleDamageBonusPctChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			                      UCancerCombatSet::GetDamageReductionPctAttribute())
		                      .AddUObject(this, &ThisClass::HandleDamageReductionPctChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetStaggerTimeAttribute())
		                      .AddUObject(this, &ThisClass::HandleStaggerTimeChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetStaggerDistanceAttribute())
		                      .AddUObject(this, &ThisClass::HandleStaggerDistanceChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetImpactAttribute())
		                      .AddUObject(this, &ThisClass::HandleImpactChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetIronBodyAttribute())
		                      .AddUObject(this, &ThisClass::HandleIronBodyChanged);

		OnAttackPowerChanged.Broadcast(this, CombatSet->GetAttackPower(), CombatSet->GetAttackPower(), nullptr);
		OnPhysicalAttackPowerChanged.Broadcast(this, CombatSet->GetPhysicalAttackPower(),
		                                       CombatSet->GetPhysicalAttackPower(), nullptr);
		OnDefenseChanged.Broadcast(this, CombatSet->GetDefense(), CombatSet->GetDefense(), nullptr);
		OnDamageBonusPctChanged.Broadcast(this, CombatSet->GetDamageBonusPct(), CombatSet->GetDamageBonusPct(),
		                                  nullptr);
		OnDamageReductionPctChanged.Broadcast(this, CombatSet->GetDamageReductionPct(),
		                                      CombatSet->GetDamageReductionPct(), nullptr);
		OnStaggerTimeChanged.Broadcast(this, CombatSet->GetStaggerTime(), CombatSet->GetStaggerTime(), nullptr);
		OnStaggerDistanceChanged.Broadcast(this, CombatSet->GetStaggerDistance(), CombatSet->GetStaggerDistance(),
		                                   nullptr);
		OnImpactChanged.Broadcast(this, CombatSet->GetImpact(), CombatSet->GetImpact(), nullptr);

		OnIronBodyChanged.Broadcast(this, CombatSet->GetIronBody(), CombatSet->GetIronBody(), nullptr);
		BaseIronBody = CombatSet->GetIronBody();
	}
}

void UCancerCombatAttributeComponent::UninitializeFromAbilitySystem()
{
	if (HealthSet)
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	if (ProgressionSet)
	{
		ProgressionSet->OnLevelChanged.RemoveAll(this);
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetAttackPowerAttribute())
		                      .RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetDefenseAttribute())
		                      .RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetDamageBonusPctAttribute())
		                      .RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			                      UCancerCombatSet::GetDamageReductionPctAttribute())
		                      .RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetStaggerTimeAttribute())
		                      .RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetStaggerDistanceAttribute())
		                      .RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetImpactAttribute())
		                      .RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCancerCombatSet::GetIronBodyAttribute())
		                      .RemoveAll(this);
	}

	HealthSet = nullptr;
	ProgressionSet = nullptr;
	CombatSet = nullptr;
	AbilitySystemComponent = nullptr;
}

void UCancerCombatAttributeComponent::RefreshAttribute()
{
	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetMaxHealth(), nullptr);
	OnHealthNormalizedChanged.Broadcast(this, GetHealthNormalized(), GetHealthNormalized(), nullptr);

	OnSoulChanged.Broadcast(this, SoulSet->GetSoul(), SoulSet->GetSoul(), nullptr);
	OnMaxSoulChanged.Broadcast(this, SoulSet->GetMaxSoul(), SoulSet->GetMaxSoul(), nullptr);
	OnSoulNormalizedChanged.Broadcast(this, GetSoulNormalized(), GetSoulNormalized(), nullptr);

	OnEnergyChanged.Broadcast(this, EnergySet->GetEnergy(), EnergySet->GetEnergy(), nullptr);
	OnMaxEnergyChanged.Broadcast(this, EnergySet->GetMaxEnergy(), EnergySet->GetMaxEnergy(), nullptr);
	OnEnergyNormalizedChanged.Broadcast(this, GetEnergyNormalized(), GetEnergyNormalized(), nullptr);


	OnStaminaChanged.Broadcast(this, StaminaSet->GetStamina(), StaminaSet->GetStamina(), nullptr);
	OnMaxStaminaChanged.Broadcast(this, StaminaSet->GetMaxStamina(), StaminaSet->GetMaxStamina(), nullptr);

	OnAttackPowerChanged.Broadcast(this, CombatSet->GetAttackPower(), CombatSet->GetAttackPower(), nullptr);
	OnPhysicalAttackPowerChanged.Broadcast(this, CombatSet->GetPhysicalAttackPower(),
	                                       CombatSet->GetPhysicalAttackPower(), nullptr);
	OnDefenseChanged.Broadcast(this, CombatSet->GetDefense(), CombatSet->GetDefense(), nullptr);
	OnDamageBonusPctChanged.Broadcast(this, CombatSet->GetDamageBonusPct(), CombatSet->GetDamageBonusPct(),
	                                  nullptr);
	OnDamageReductionPctChanged.Broadcast(this, CombatSet->GetDamageReductionPct(),
	                                      CombatSet->GetDamageReductionPct(), nullptr);
	OnStaggerTimeChanged.Broadcast(this, CombatSet->GetStaggerTime(), CombatSet->GetStaggerTime(), nullptr);
	OnStaggerDistanceChanged.Broadcast(this, CombatSet->GetStaggerDistance(), CombatSet->GetStaggerDistance(),
	                                   nullptr);
	OnImpactChanged.Broadcast(this, CombatSet->GetImpact(), CombatSet->GetImpact(), nullptr);

	OnIronBodyChanged.Broadcast(this, CombatSet->GetIronBody(), CombatSet->GetIronBody(), nullptr);
	BaseIronBody = CombatSet->GetIronBody();
}

#pragma region 属性
float UCancerCombatAttributeComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float UCancerCombatAttributeComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float UCancerCombatAttributeComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();

		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}

float UCancerCombatAttributeComponent::GetSoul() const
{
	return (SoulSet ? SoulSet->GetSoul() : 0.0f);
}

float UCancerCombatAttributeComponent::GetMaxSoul() const
{
	return (SoulSet ? SoulSet->GetMaxSoul() : 0.0f);
}

float UCancerCombatAttributeComponent::GetSoulNormalized() const
{
	if (SoulSet)
	{
		const float Soul = SoulSet->GetSoul();
		const float MaxSoul = SoulSet->GetMaxSoul();

		return ((MaxSoul > 0.0f) ? (Soul / MaxSoul) : 0.0f);
	}

	return 0.0f;
}

float UCancerCombatAttributeComponent::GetStamina() const
{
	return (StaminaSet ? StaminaSet->GetStamina() : 0.0f);
}

float UCancerCombatAttributeComponent::GetMaxStamina() const
{
	return (StaminaSet ? StaminaSet->GetMaxStamina() : 0.0f);
}

float UCancerCombatAttributeComponent::GetStaminaNormalized() const
{
	if (StaminaSet)
	{
		const float Stamina = StaminaSet->GetStamina();
		const float MaxStamina = StaminaSet->GetMaxStamina();

		return ((MaxStamina > 0.0f) ? (Stamina / MaxStamina) : 0.0f);
	}

	return 0.0f;
}

float UCancerCombatAttributeComponent::GetEnergy() const
{
	return (EnergySet ? EnergySet->GetEnergy() : 0.0f);
}

float UCancerCombatAttributeComponent::GetMaxEnergy() const
{
	return (EnergySet ? EnergySet->GetMaxEnergy() : 0.0f);
}

float UCancerCombatAttributeComponent::GetEnergyNormalized() const
{
	if (EnergySet)
	{
		const float Energy = EnergySet->GetEnergy();
		const float MaxEnergy = EnergySet->GetMaxEnergy();

		return ((MaxEnergy > 0.0f) ? (Energy / MaxEnergy) : 0.0f);
	}

	return 0.0f;
}

float UCancerCombatAttributeComponent::GetLevel() const
{
	return (ProgressionSet ? ProgressionSet->GetLevel() : 0.0f);
}

float UCancerCombatAttributeComponent::GetMaxLevel() const
{
	return (ProgressionSet ? ProgressionSet->GetMaxLevel() : 0.0f);
}

float UCancerCombatAttributeComponent::GetExp() const
{
	return (ProgressionSet ? ProgressionSet->GetExp() : 0.0f);
}

float UCancerCombatAttributeComponent::GetMaxExp() const
{
	return (ProgressionSet ? ProgressionSet->GetMaxExp() : 0.0f);
}

float UCancerCombatAttributeComponent::GetAttackPower() const
{
	return (CombatSet ? CombatSet->GetAttackPower() : 0.0f);
}

float UCancerCombatAttributeComponent::GetPhysicalAttackPower() const
{
	return (CombatSet ? CombatSet->GetPhysicalAttackPower() : 0.0f);
}

float UCancerCombatAttributeComponent::GetDefense() const
{
	return (CombatSet ? CombatSet->GetDefense() : 0.0f);
}

float UCancerCombatAttributeComponent::GetDamageBonusPct() const
{
	return (CombatSet ? CombatSet->GetDamageBonusPct() : 0.0f);
}

float UCancerCombatAttributeComponent::GetDamageReductionPct() const
{
	return (CombatSet ? CombatSet->GetDamageReductionPct() : 0.0f);
}

float UCancerCombatAttributeComponent::GetStaggerTime() const
{
	return (CombatSet ? CombatSet->GetStaggerTime() : 0.0f);
}

float UCancerCombatAttributeComponent::GetStaggerDistance() const
{
	return (CombatSet ? CombatSet->GetStaggerDistance() : 0.0f);
}

float UCancerCombatAttributeComponent::GetImpact() const
{
	return (CombatSet ? CombatSet->GetImpact() : 0.0f);
}

float UCancerCombatAttributeComponent::GetIronBody() const
{
	return (CombatSet ? CombatSet->GetIronBody() : 0.0f);
}

float UCancerCombatAttributeComponent::GetBaseIronBody() const
{
	return BaseIronBody;
}


void UCancerCombatAttributeComponent::StartDeath(const FGameplayEventData& EventData)
{
	if (DeathState != ECancerDeathState::NotDead)
	{
		return;
	}

	DeathState = ECancerDeathState::DeathStarted;

	if (AbilitySystemComponent)
		AbilitySystemComponent->SetLooseGameplayTagCount(Tag_Combat_Event_Startdyging, 1);

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner, EventData);
}

void UCancerCombatAttributeComponent::FinishDeath(const FGameplayEventData& EventData)
{
	if (DeathState != ECancerDeathState::DeathStarted)
	{
		return;
	}

	DeathState = ECancerDeathState::DeathFinished;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(Tag_Combat_Event_Deaddead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner, EventData);

	Owner->ForceNetUpdate();
}



void UCancerCombatAttributeComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                                          const FGameplayEffectSpec* DamageEffectSpec,
                                                          float DamageMagnitude, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageCauser);
}

void UCancerCombatAttributeComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                                             const FGameplayEffectSpec* DamageEffectSpec,
                                                             float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageCauser);
}

void UCancerCombatAttributeComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
                                                        const FGameplayEffectSpec* DamageEffectSpec,
                                                        float DamageMagnitude, float OldValue, float NewValue)
{
	FGameplayEventData Payload;
	Payload.EventTag = Tag_Combat_Event_AbilityTrigger_Death;
	Payload.Instigator = DamageInstigator;
	Payload.Target = AbilitySystemComponent->GetAvatarActor();
	Payload.OptionalObject = DamageEffectSpec->GetEffectContext().GetSourceObject();
	Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
	Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
	Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
	Payload.EventMagnitude = DamageMagnitude;
	//死亡事件
	if (AbilitySystemComponent && DamageEffectSpec)
	{
		AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
	StartDeath(Payload);
}


void UCancerCombatAttributeComponent::HandleSoulChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                                        const FGameplayEffectSpec* DamageEffectSpec,
                                                        float DamageMagnitude, float OldValue, float NewValue)
{
	OnSoulChanged.Broadcast(this, OldValue, NewValue, DamageCauser);
}

void UCancerCombatAttributeComponent::HandleMaxSoulChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                                           const FGameplayEffectSpec* DamageEffectSpec,
                                                           float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxSoulChanged.Broadcast(this, OldValue, NewValue, DamageCauser);
}

void UCancerCombatAttributeComponent::HandleOutOfSoul(AActor* DamageInstigator, AActor* DamageCauser,
                                                      const FGameplayEffectSpec* DamageEffectSpec,
                                                      float DamageMagnitude, float OldValue, float NewValue)
{
	if (AbilitySystemComponent && DamageEffectSpec)
	{
		FGameplayEventData Payload;
		FGameplayTag EventTag = Tag_Combat_Event_AbilityTrigger_Weak;
		/*if (DamageEffectSpec->GetDynamicAssetTags().HasTagExact(Tag_Combat_Event_BeParried))
		{
		    /*EventTag = Tag_Combat_Event_BeParried;#1#
		}*/
		Payload.EventTag = EventTag;
		Payload.Instigator = DamageInstigator;
		Payload.Target = AbilitySystemComponent->GetAvatarActor();
		Payload.OptionalObject = DamageEffectSpec->GetEffectContext().GetSourceObject();
		Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
		Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
		Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
		Payload.EventMagnitude = DamageMagnitude;
		AbilitySystemComponent->HandleGameplayEvent(EventTag, &Payload);
	}
}

void UCancerCombatAttributeComponent::HandleEnergyChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                                          const FGameplayEffectSpec* DamageEffectSpec,
                                                          float DamageMagnitude, float OldValue, float NewValue)
{
	OnEnergyChanged.Broadcast(this, OldValue, NewValue, DamageCauser);
}

void UCancerCombatAttributeComponent::HandleMaxEnergyChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                                             const FGameplayEffectSpec* DamageEffectSpec,
                                                             float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxEnergyChanged.Broadcast(this, OldValue, NewValue, DamageCauser);
}

void UCancerCombatAttributeComponent::HandleOutOfEnergy(AActor* DamageInstigator, AActor* DamageCauser,
                                                        const FGameplayEffectSpec* DamageEffectSpec,
                                                        float DamageMagnitude, float OldValue, float NewValue)
{
}

void UCancerCombatAttributeComponent::HandleStaminaChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                                           const FGameplayEffectSpec* DamageEffectSpec,
                                                           float DamageMagnitude, float OldValue, float NewValue)
{
	OnStaminaChanged.Broadcast(this, OldValue, NewValue, DamageCauser);
}

void UCancerCombatAttributeComponent::HandleMaxStaminaChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                                              const FGameplayEffectSpec* DamageEffectSpec,
                                                              float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxStaminaChanged.Broadcast(this, OldValue, NewValue, DamageCauser);
}

void UCancerCombatAttributeComponent::HandleOutOfStamina(AActor* DamageInstigator, AActor* DamageCauser,
                                                         const FGameplayEffectSpec* DamageEffectSpec,
                                                         float DamageMagnitude, float OldValue, float NewValue)
{
}


void UCancerCombatAttributeComponent::HandleLevelChanged(AActor* EffectInstigator, AActor* EffectCauser,
                                                         const FGameplayEffectSpec* EffectSpec, float EffectMagnitude,
                                                         float OldValue, float NewValue)
{
	OnLevelChanged.Broadcast(this, OldValue, NewValue, EffectCauser);
}

void UCancerCombatAttributeComponent::HandleAttackPowerChanged(const struct FOnAttributeChangeData& ChangeData)
{
	OnAttackPowerChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UCancerCombatAttributeComponent::HandlePhysicalAttackPowerChanged(const struct FOnAttributeChangeData& ChangeData)
{
	OnPhysicalAttackPowerChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UCancerCombatAttributeComponent::HandleDefenseChanged(const struct FOnAttributeChangeData& ChangeData)
{
	OnDefenseChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UCancerCombatAttributeComponent::HandleDamageBonusPctChanged(const struct FOnAttributeChangeData& ChangeData)
{
	OnDamageBonusPctChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UCancerCombatAttributeComponent::HandleDamageReductionPctChanged(const struct FOnAttributeChangeData& ChangeData)
{
	OnDamageReductionPctChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UCancerCombatAttributeComponent::HandleStaggerTimeChanged(const struct FOnAttributeChangeData& ChangeData)
{
	OnStaggerTimeChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UCancerCombatAttributeComponent::HandleStaggerDistanceChanged(const struct FOnAttributeChangeData& ChangeData)
{
	OnStaggerDistanceChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UCancerCombatAttributeComponent::HandleImpactChanged(const struct FOnAttributeChangeData& ChangeData)
{
	OnImpactChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UCancerCombatAttributeComponent::HandleIronBodyChanged(const struct FOnAttributeChangeData& ChangeData)
{
	OnIronBodyChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}
#pragma endregion
