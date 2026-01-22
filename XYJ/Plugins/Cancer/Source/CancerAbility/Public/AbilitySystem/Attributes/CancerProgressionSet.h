#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CancerAttributeSet.h"
#include "CancerProgressionSet.generated.h"

/**
 * 角色成长属性集:Level、经验值
 */
UCLASS(BlueprintType)
class CANCERABILITY_API UCancerProgressionSet : public UCancerAttributeSet
{
	GENERATED_BODY()

public:
	UCancerProgressionSet();

	ATTRIBUTE_ACCESSORS(UCancerProgressionSet, Level);
	ATTRIBUTE_ACCESSORS(UCancerProgressionSet, MaxLevel);
	ATTRIBUTE_ACCESSORS(UCancerProgressionSet, Exp);
	ATTRIBUTE_ACCESSORS(UCancerProgressionSet, MaxExp);

	mutable FCancerAttributeEvent OnLevelChanged;
	mutable FCancerAttributeEvent OnMaxLevelChanged;
	mutable FCancerAttributeEvent OnExpChanged;
	mutable FCancerAttributeEvent OnMaxExpChanged;

protected:
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Progression", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Level = 1.f;
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Progression", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MaxLevel = 100.f;
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Progression", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Exp = 0.f;
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Progression", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MaxExp = 100.f;
};
