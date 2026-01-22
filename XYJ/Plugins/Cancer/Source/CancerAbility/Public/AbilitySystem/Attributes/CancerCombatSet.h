#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CancerAttributeSet.h"
#include "CancerCombatSet.generated.h"

// ============================================================================
//  战斗属性集（示例）
//  用途：存放与战斗结算相关的基础数值，如基础伤害、基础治疗等。
// ============================================================================
UCLASS()
class CANCERABILITY_API UCancerCombatSet : public UCancerAttributeSet
{
	GENERATED_BODY()

public:
	UCancerCombatSet();

	// 基础伤害、气力伤害、防御、伤害加成、伤害减免
	ATTRIBUTE_ACCESSORS(UCancerCombatSet, AttackPower);
	ATTRIBUTE_ACCESSORS(UCancerCombatSet, PhysicalAttackPower);
	ATTRIBUTE_ACCESSORS(UCancerCombatSet, Defense);
	ATTRIBUTE_ACCESSORS(UCancerCombatSet, DamageBonusPct);
	ATTRIBUTE_ACCESSORS(UCancerCombatSet, DamageReductionPct);
	ATTRIBUTE_ACCESSORS(UCancerCombatSet, IronBody); 
	ATTRIBUTE_ACCESSORS(UCancerCombatSet, StaggerTime); 
	ATTRIBUTE_ACCESSORS(UCancerCombatSet, StaggerDistance);
	ATTRIBUTE_ACCESSORS(UCancerCombatSet, Impact);


protected:
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Cancer|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackPower;
	
	UPROPERTY(BlueprintReadOnly, Category = "Cancer|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData PhysicalAttackPower;

	UPROPERTY(BlueprintReadOnly, Category = "Cancer|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Defense;

	UPROPERTY(BlueprintReadOnly, Category = "Cancer|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageBonusPct;

	UPROPERTY(BlueprintReadOnly, Category = "Cancer|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageReductionPct;

	UPROPERTY(BlueprintReadOnly, Category = "Cancer|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData StaggerTime;

	UPROPERTY(BlueprintReadOnly, Category = "Cancer|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData StaggerDistance;

	UPROPERTY(BlueprintReadOnly, Category = "Cancer|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Impact;
	
	UPROPERTY(BlueprintReadOnly, Category = "Cancer|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IronBody;
};
