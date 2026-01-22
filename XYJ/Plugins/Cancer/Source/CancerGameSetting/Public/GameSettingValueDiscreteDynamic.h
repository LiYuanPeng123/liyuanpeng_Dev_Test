#pragma once

#include "CoreMinimal.h"
#include "GameSetting.h"
#include "GameSettingValueDiscreteDynamic.generated.h"

// 动态离散 Getter 委托
DECLARE_DELEGATE_RetVal(int32, FGameSettingDiscreteGetter);
// 动态离散 Setter 委托
DECLARE_DELEGATE_OneParam(FGameSettingDiscreteSetter, int32);

/**
 * 动态离散选择设置类
 * 通过委托直接连接到游戏系统的 Getter/Setter
 */
UCLASS(BlueprintType, Blueprintable, meta = (Category = "Game Settings"))
class CANCERGAMESETTING_API UGameSettingValueDiscreteDynamic : public UGameSettingValueDiscrete
{
    GENERATED_BODY()

public:
    UGameSettingValueDiscreteDynamic();

    /**
     * 设置动态 Getter
     */
    void SetDynamicGetter(const FGameSettingDiscreteGetter& InGetter) { DynamicGetter = InGetter; }

    /**
     * 设置动态 Setter
     */
    void SetDynamicSetter(const FGameSettingDiscreteSetter& InSetter) { DynamicSetter = InSetter; }

    // 重写基类方法
    virtual int32 GetDiscreteOptionIndex() const override;
    virtual void SetDiscreteOptionByIndex(int32 Index) override;
    virtual void Apply() override;

    virtual void RestoreToInitial() override { SetDiscreteOptionByIndex(DefaultIndex);}
protected:
    /** 动态获取器 */
    FGameSettingDiscreteGetter DynamicGetter;
    
    /** 动态设置器 */
    FGameSettingDiscreteSetter DynamicSetter;
};