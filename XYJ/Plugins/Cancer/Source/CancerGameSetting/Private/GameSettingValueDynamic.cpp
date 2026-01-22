#include "GameSettingValueDynamic.h"

UGameSettingValueDynamic::UGameSettingValueDynamic()
{
    // 初始化
}

float UGameSettingValueDynamic::GetValue() const
{
    if (DynamicGetter.IsBound())
    {
        return DynamicGetter.Execute();
    }
    return Super::GetValue();
}

void UGameSettingValueDynamic::SetValue(float InValue)
{
    Super::SetValue(InValue);
    
    // 如果有动态 Setter，立即应用
    if (DynamicSetter.IsBound())
    {
        DynamicSetter.Execute(InValue);
    }
}

void UGameSettingValueDynamic::Apply()
{
    if (DynamicSetter.IsBound())
    {
        DynamicSetter.Execute(GetValue());
    }
    
    Super::Apply();
}