#include "AbilitySystem/Attributes/CancerProgressionSet.h"
#include "CancerAbilitySystemComponent.h"
#include "Engine/CurveTable.h"

UCancerProgressionSet::UCancerProgressionSet()
{
}

void UCancerProgressionSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UCancerProgressionSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UCancerProgressionSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetLevelAttribute())
	{
		if (auto ASC = GetCancerAbilitySystemComponent())
		{
			ASC->SetLevel(static_cast<int32>(NewValue));
		}
		OnLevelChanged.Broadcast(nullptr, nullptr, nullptr, 0.f, OldValue, NewValue);
	}
	if (Attribute == GetMaxLevelAttribute())
	{
		OnMaxLevelChanged.Broadcast(nullptr, nullptr, nullptr, 0.f, OldValue, NewValue);
	}
	// 经验值变化：广播事件并处理升级
	if (Attribute == GetExpAttribute())
	{
		OnExpChanged.Broadcast(nullptr, nullptr, nullptr, 0.f, OldValue, NewValue);

		float CurrentExp = GetExp();
		int32 Safety = 0;
		while (true)
		{
			const float CurrentLevel = GetLevel();
			const float CurrentMaxExp = GetMaxExp();

			if (CurrentExp < CurrentMaxExp)
			{
				break;
			}

			// 升级：消耗当前经验并提升等级
			CurrentExp -= CurrentMaxExp;
			SetLevel(CurrentLevel + 1.f);

			// 等级变化后应用派生属性
			if (auto ASC = GetCancerAbilitySystemComponent())
			{
				ASC->SetLevel(static_cast<int32>(GetLevel()));
			}

			// 写回剩余经验，支持多级连升
			SetExp(CurrentExp);

			if (++Safety > 100)
			{
				break; // 防御性保护，避免异常情况下死循环
			}
		}
	}

	// 当 MaxExp 改变时，确保显示不溢出（多级连升逻辑在 Exp 分支处理）
	if (Attribute == GetMaxExpAttribute())
	{
		if (GetExp() > GetMaxExp())
		{
			SetExp(GetMaxExp());
		}
	}
}

void UCancerProgressionSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.f, GetMaxLevel());
	}
	else if (Attribute == GetExpAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetMaxExpAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
}
