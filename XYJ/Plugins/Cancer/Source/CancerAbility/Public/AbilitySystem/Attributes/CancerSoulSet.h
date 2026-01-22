
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CancerAttributeSet.h"
#include "CancerSoulSet.generated.h"

/**
 * 精神力
 */
UCLASS()
class CANCERABILITY_API UCancerSoulSet : public UCancerAttributeSet
{
	GENERATED_BODY()
public:
	UCancerSoulSet();
	ATTRIBUTE_ACCESSORS(UCancerSoulSet, Soul);
	ATTRIBUTE_ACCESSORS(UCancerSoulSet, MaxSoul);
	
	mutable FCancerAttributeEvent OnSoulChanged;
	mutable FCancerAttributeEvent OnMaxSoulChanged;
	mutable FCancerAttributeEvent OnOutOfSoul;

protected:
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;


	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
private:
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Stamina", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Soul;
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Stamina", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MaxSoul;
	
	bool bOutOfSoul;
	
	float MaxSoulBeforeAttributeChange;
	
	float SoulBeforeAttributeChange;
	
};
