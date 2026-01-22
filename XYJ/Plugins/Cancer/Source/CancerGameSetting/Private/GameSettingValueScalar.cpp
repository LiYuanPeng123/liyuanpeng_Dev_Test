#include "GameSettingValueScalar.h"
#include "Engine/Engine.h"

UGameSettingValueScalar::UGameSettingValueScalar()
{
	MinValue = 0.0f;
	MaxValue = 100.0f;
	StepSize = 1.0f;
	CurrentValue = 50.0f;
	DefaultValue = 50.0f;
	InitialValue = 50.0f;
}

void UGameSettingValueScalar::SetValue(float InValue)
{
	float NewValue = ApplyStepSize(ClampValue(InValue));
	
	if (FMath::IsNearlyEqual(CurrentValue, NewValue))
	{
		return;
	}

	CurrentValue = NewValue;
	
	// 触发设置变化事件
	OnSettingChanged.Broadcast(this, EGameSettingChangeReason::Change);
}

float UGameSettingValueScalar::GetNormalizedValue() const
{
	if (FMath::IsNearlyEqual(MaxValue, MinValue))
	{
		return 0.0f;
	}
	
	return (CurrentValue - MinValue) / (MaxValue - MinValue);
}

void UGameSettingValueScalar::SetNormalizedValue(float NormalizedValue)
{
	float ClampedNormalized = FMath::Clamp(NormalizedValue, 0.0f, 1.0f);
	float NewValue = MinValue + (ClampedNormalized * (MaxValue - MinValue));
	SetValue(NewValue);
}

void UGameSettingValueScalar::StoreInitial()
{
	InitialValue = CurrentValue;
}

void UGameSettingValueScalar::ResetToDefault()
{
	SetValue(DefaultValue);
}

void UGameSettingValueScalar::RestoreToInitial()
{
	SetValue(InitialValue);
}

void UGameSettingValueScalar::OnApply()
{
	// 应用设置到游戏系统
	// 这里可以根据具体的设置类型进行相应的应用逻辑
	// 例如：音量设置可以调用音频系统的API
	
	// 存储当前值作为新的初始值
	StoreInitial();
	
	// 触发应用事件
	OnSettingApplied.Broadcast(this);
}


FString UGameSettingValueScalar::GetAnalyticsValue() const
{
	return FString::Printf(TEXT("%.2f"), CurrentValue);
}

float UGameSettingValueScalar::ClampValue(float InValue) const
{
	return FMath::Clamp(InValue, MinValue, MaxValue);
}

float UGameSettingValueScalar::ApplyStepSize(float InValue) const
{
	if (StepSize <= 0.0f)
	{
		return InValue;
	}

	// 计算最接近的步长值
	float Steps = FMath::RoundToFloat((InValue - MinValue) / StepSize);
	return MinValue + (Steps * StepSize);
}