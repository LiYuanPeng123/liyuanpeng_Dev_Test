#pragma once

#include "CoreMinimal.h"
#include "GameSetting.h"
#include "GameSettingValueScalar.h"
#include "GameSettingValueDynamic.generated.h"

// 动态 Getter 委托
DECLARE_DELEGATE_RetVal(float, FGameSettingDynamicGetter);
// 动态 Setter 委托
DECLARE_DELEGATE_OneParam(FGameSettingDynamicSetter, float);

/**
 * 动态数值设置基类
 * 通过委托直接连接到游戏系统的 Getter/Setter
 */
UCLASS(BlueprintType, Blueprintable)
class CANCERGAMESETTING_API UGameSettingValueDynamic : public UGameSettingValueScalar
{
    GENERATED_BODY()

public:
    UGameSettingValueDynamic();

    /**
     * 设置动态 Getter
     */
    void SetDynamicGetter(const FGameSettingDynamicGetter& InGetter) { DynamicGetter = InGetter; }

    /**
     * 设置动态 Setter
     */
    void SetDynamicSetter(const FGameSettingDynamicSetter& InSetter) { DynamicSetter = InSetter; }

    // 重写基类方法
    virtual float GetValue() const override;
    virtual void SetValue(float InValue) override;
    virtual void Apply() override;
 
protected:
    /** 动态获取器 */
    FGameSettingDynamicGetter DynamicGetter;
    
    /** 动态设置器 */
    FGameSettingDynamicSetter DynamicSetter;
};