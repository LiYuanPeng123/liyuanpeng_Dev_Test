#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CancerAttributeSet.h"
#include "CancerEnergySet.generated.h"

/**
 * 法力（Mana）属性集
 */
UCLASS(BlueprintType)
class CANCERABILITY_API UCancerEnergySet : public UCancerAttributeSet
{
    GENERATED_BODY()

public:
    UCancerEnergySet();

    //法力值、最大法力值
    ATTRIBUTE_ACCESSORS(UCancerEnergySet, Energy);
    ATTRIBUTE_ACCESSORS(UCancerEnergySet, MaxEnergy);
    mutable FCancerAttributeEvent OnEnergyChanged;
    mutable FCancerAttributeEvent OnMaxEnergyChanged;
    mutable FCancerAttributeEvent OnOutOfEnergy;

protected:
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

    void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	float MaxEnergyBeforeAttributeChange;
	float EnergyBeforeAttributeChange;
    UPROPERTY(BlueprintReadOnly, Category="Cancer|Mana", Meta=(AllowPrivateAccess=true))
    FGameplayAttributeData Energy;
    UPROPERTY(BlueprintReadOnly, Category="Cancer|Mana", Meta=(AllowPrivateAccess=true))
    FGameplayAttributeData MaxEnergy;
    bool bOutOfMana = false;
};