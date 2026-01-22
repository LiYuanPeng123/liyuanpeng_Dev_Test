#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CancerAttributeSet.h"
#include "CancerItemAttributeSet.generated.h"

/**
 * 装备（来源侧）属性集：
 */
UCLASS(BlueprintType)
class CANCERABILITY_API UCancerItemAttributeSet : public UCancerAttributeSet
{
    GENERATED_BODY()

public:
    UCancerItemAttributeSet();

    // 攻击力、防御力、伤害加成、伤害免疫、武器等级
    ATTRIBUTE_ACCESSORS(UCancerItemAttributeSet, AttackPowerBonus);
    ATTRIBUTE_ACCESSORS(UCancerItemAttributeSet, DefenseBonus);
    ATTRIBUTE_ACCESSORS(UCancerItemAttributeSet, DamageBonusPctBonus);
    ATTRIBUTE_ACCESSORS(UCancerItemAttributeSet, DamageReductionPctBonus);
    ATTRIBUTE_ACCESSORS(UCancerItemAttributeSet, WeaponLevel);

protected:
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
    UPROPERTY(BlueprintReadOnly, Category="Cancer|Item", Meta=(AllowPrivateAccess=true))
    FGameplayAttributeData AttackPowerBonus;
    UPROPERTY(BlueprintReadOnly, Category="Cancer|Item", Meta=(AllowPrivateAccess=true))
    FGameplayAttributeData DefenseBonus;
    UPROPERTY(BlueprintReadOnly, Category="Cancer|Item", Meta=(AllowPrivateAccess=true))
    FGameplayAttributeData DamageBonusPctBonus;
    UPROPERTY(BlueprintReadOnly, Category="Cancer|Item", Meta=(AllowPrivateAccess=true))
    FGameplayAttributeData DamageReductionPctBonus;
    UPROPERTY(BlueprintReadOnly, Category="Cancer|Item", Meta=(AllowPrivateAccess=true))
    FGameplayAttributeData WeaponLevel;
};