#include "GameSettingValueDiscreteDynamic.h"

UGameSettingValueDiscreteDynamic::UGameSettingValueDiscreteDynamic()
{
    // 初始化
}

int32 UGameSettingValueDiscreteDynamic::GetDiscreteOptionIndex() const
{
    if (DynamicGetter.IsBound())
    {
        return DynamicGetter.Execute();
    }
    return Super::GetDiscreteOptionIndex();
}

void UGameSettingValueDiscreteDynamic::SetDiscreteOptionByIndex(int32 Index)
{
    Super::SetDiscreteOptionByIndex(Index);
    
    // 如果有动态 Setter，立即应用
    if (DynamicSetter.IsBound())
    {
        DynamicSetter.Execute(Index);
    }
}

void UGameSettingValueDiscreteDynamic::Apply()
{
    if (DynamicSetter.IsBound())
    {
        DynamicSetter.Execute(GetDiscreteOptionIndex());
    }
    
    Super::Apply();
}