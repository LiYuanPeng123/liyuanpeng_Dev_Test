#include "AbilitySystem/Attributes/CancerHealthSet.h"
#include "AbilitySystemComponent.h"
#include "CancerAbilitySystemComponent.h"
#include "CancerNativeGamePlayTag.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "Statics/CancerStaticsSubsystem.h"

UCancerHealthSet::UCancerHealthSet()
{
	bOutOfHealth = false;
	MaxHealthBeforeAttributeChange = 0.0f;
	HealthBeforeAttributeChange = 0.0f;
}

bool UCancerHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	// 调用父类实现，如果父类处理失败则直接返回
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// 记录变化前数值（用于事件）
	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();

	return true;
}

void UCancerHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		//免疫伤害
		if (UCancerAbilitySystemComponent* CancerASC = GetCancerAbilitySystemComponent())
		{
			if (CancerASC->HasTag(Tag_Combat_State_UnlimitedHealth, true))
			{
				SetDamage(0.0f);
				return;
			}
		}
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), 0, GetMaxHealth()));
		SetDamage(0.0f);

		if (UCancerAbilitySystemComponent* CancerASC = GetCancerAbilitySystemComponent())
		{
			FGameplayCueParameters Params;
			Params.RawMagnitude = Data.EvaluatedData.Magnitude;
			
			// 伤害统计
			UCancerStaticsSubsystem::StaticDamage(Instigator,Data.EvaluatedData.Magnitude);
			
			Params.EffectContext = EffectContext;
			Params.AggregatedSourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Params.AggregatedTargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			Params.Instigator = Instigator;
			Params.EffectCauser = Causer;

			if (const FHitResult* HR = EffectContext.GetHitResult())
			{
				Params.Location = HR->Location;
				Params.Normal = HR->Normal;
			}
			CancerASC->ExecuteGameplayCue(GameplayCue_HitNumber, Params);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), 0, GetMaxHealth()));
		SetHealing(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		                             MaxHealthBeforeAttributeChange, GetMaxHealth());
	}
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		                          HealthBeforeAttributeChange, GetHealth());
	}
	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		                        HealthBeforeAttributeChange, GetHealth());
	}
	bOutOfHealth = (GetHealth() <= 0.0f);
}

void UCancerHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UCancerHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UCancerHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		if (GetHealth() > NewValue)
		{
			UCancerAbilitySystemComponent* CancerASC = GetCancerAbilitySystemComponent();
			check(CancerASC);

			CancerASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void UCancerHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
