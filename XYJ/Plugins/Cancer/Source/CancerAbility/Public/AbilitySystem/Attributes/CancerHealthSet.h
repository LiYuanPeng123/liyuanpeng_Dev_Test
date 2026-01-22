
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CancerAttributeSet.h"
#include "CancerHealthSet.generated.h"



/**
 * 生命值属性集
 */
UCLASS(BlueprintType)
class CANCERABILITY_API UCancerHealthSet : public UCancerAttributeSet
{
	GENERATED_BODY()

public:
	UCancerHealthSet();

	// 属性访问器宏，为每个属性生成 Get/Set 函数
	ATTRIBUTE_ACCESSORS(UCancerHealthSet, Health);          // 当前生命值
	ATTRIBUTE_ACCESSORS(UCancerHealthSet, MaxHealth);       // 最大生命值
	ATTRIBUTE_ACCESSORS(UCancerHealthSet, Healing);         // 治疗量
	ATTRIBUTE_ACCESSORS(UCancerHealthSet, Damage);          // 伤害量
	
	// 生命值变化事件
	mutable FCancerAttributeEvent OnHealthChanged;
	
	// 最大生命值变化事件
	mutable FCancerAttributeEvent OnMaxHealthChanged;
	
	// 生命值降为0时触发的事件
	mutable FCancerAttributeEvent OnOutOfHealth;


protected:

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Cancer|Health",
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, Category = "Cancer|Health",
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	//是否死亡
	bool bOutOfHealth;
	
	float MaxHealthBeforeAttributeChange;
	
	float HealthBeforeAttributeChange;
	
	// 治疗量属性（临时值，处理后会被清空）
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Healing;
	
	// 伤害量属性（临时值，处理后会被清空）
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;
};
