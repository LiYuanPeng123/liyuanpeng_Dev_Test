#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CancerAttributeSet.h"
#include "CancerStaminaSet.generated.h"

/**
 * 耐力（Stamina）
 */
UCLASS(BlueprintType)
class CANCERABILITY_API UCancerStaminaSet : public UCancerAttributeSet
{
	GENERATED_BODY()

public:
	UCancerStaminaSet();

	//耐力、最大耐力值
	ATTRIBUTE_ACCESSORS(UCancerStaminaSet, Stamina);
	ATTRIBUTE_ACCESSORS(UCancerStaminaSet, MaxStamina);
	mutable FCancerAttributeEvent OnStaminaChanged;
	mutable FCancerAttributeEvent OnMaxStaminaChanged;
	mutable FCancerAttributeEvent OnOutOfStamina;

protected:
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Stamina", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Stamina;
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Stamina", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MaxStamina;
	bool bOutOfStamina = false;
};
