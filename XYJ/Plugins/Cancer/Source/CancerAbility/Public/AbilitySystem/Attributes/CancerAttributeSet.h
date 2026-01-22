
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"

#include "CancerAttributeSet.generated.h"


// ============================================================================
//  属性工具宏
//  用于快速生成 GameplayAttribute 的 Getter/Setter/Init 等标准访问接口。
//  使用方式：在派生的 AttributeSet 类中写
//    ATTRIBUTE_ACCESSORS(类名, 属性名);
//  即可获得：
//   - GAMEPLAYATTRIBUTE_PROPERTY_GETTER
//   - GAMEPLAYATTRIBUTE_VALUE_GETTER
//   - GAMEPLAYATTRIBUTE_VALUE_SETTER
//   - GAMEPLAYATTRIBUTE_VALUE_INITTER
// ============================================================================
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class UCancerAbilitySystemComponent;
struct FGameplayEffectSpec;
// 属性变化事件：携带施加者、来源体、效果规格、变化幅度、旧值、新值等信息
DECLARE_MULTICAST_DELEGATE_SixParams(FCancerAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/,
                                     const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/,
                                     float /*OldValue*/, float /*NewValue*/);
/**
 * 属性基类
 */
UCLASS()
class CANCERABILITY_API UCancerAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UCancerAttributeSet();

    virtual  UWorld* GetWorld() const override;

    // 获取项目自定义的 AbilitySystemComponent
    UCancerAbilitySystemComponent* GetCancerAbilitySystemComponent() const;
};
