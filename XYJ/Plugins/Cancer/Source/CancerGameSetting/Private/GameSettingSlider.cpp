

#include "GameSettingSlider.h"
#include "Math/UnrealMathUtility.h"

/**
 * 构造函数
 * 初始化滑块设置项的默认参数
 */
UGameSettingSlider::UGameSettingSlider()
{
	// 初始化默认值
	MinValue = 0.0f;          // 滑块最小值
	MaxValue = 100.0f;        // 滑块最大值
	StepSize = 1.0f;          // 滑块步长
	CurrentValue = 50.0f;     // 当前值（默认为中间值）
	DefaultValue = 50.0f;     // 默认值
	InitialValue = 50.0f;     // 初始值（用于恢复操作）
	DisplayFormat = FText::FromString(TEXT("{0}"));  // 显示格式，{0}为占位符
}

/**
 * 设置滑块的取值范围和步长
 * @param InMinValue 最小值
 * @param InMaxValue 最大值  
 * @param InStepSize 步长（必须为正数）
 */
void UGameSettingSlider::SetRange(float InMinValue, float InMaxValue, float InStepSize)
{
	MinValue = InMinValue;
	MaxValue = FMath::Max(InMinValue, InMaxValue); // 确保最大值不小于最小值
	StepSize = FMath::Max(0.01f, InStepSize); // 确保步长为正数，最小为0.01
	
	// 重新约束当前值和默认值到新的范围内
	CurrentValue = ClampValue(CurrentValue);
	DefaultValue = ClampValue(DefaultValue);
}

/**
 * 设置滑块的当前值
 * 会自动应用范围限制和步长约束，并触发变更事件
 * @param InValue 要设置的新值
 */
void UGameSettingSlider::SetValue(float InValue)
{
	// 先限制范围，再应用步长约束
	float NewValue = ApplyStepSize(ClampValue(InValue));
	
	// 只有当值真正发生变化时才触发事件（避免浮点数精度问题）
	if (!FMath::IsNearlyEqual(NewValue, CurrentValue, 0.001f))
	{
		CurrentValue = NewValue;
		NotifySettingChanged(EGameSettingChangeReason::Change);
	}
}

/**
 * 获取格式化后的显示文本
 * 根据DisplayFormat格式化当前值
 * @return 格式化后的显示文本
 */
FText UGameSettingSlider::GetDisplayText() const
{
	// 使用格式化字符串显示当前值
	// 例如：DisplayFormat为"{0}%"时，显示"50%"
	return FText::Format(DisplayFormat, FText::AsNumber(CurrentValue));
}

/**
 * 设置数值的显示格式
 * @param Format 格式化字符串，使用{0}作为数值占位符
 *               例如："{0}%"、"{0}px"、"音量: {0}"等
 */
void UGameSettingSlider::SetDisplayFormat(const FText& Format)
{
	DisplayFormat = Format;
}

/**
 * 设置默认值
 * 会自动应用范围限制
 * @param InDefaultValue 新的默认值
 */
void UGameSettingSlider::SetDefaultValue(float InDefaultValue)
{
	DefaultValue = ClampValue(InDefaultValue);
}

/**
 * 获取标准化的值（0-1范围）
 * 用于UI滑块控件的绑定
 * @return 0-1范围内的标准化值
 */
float UGameSettingSlider::GetNormalizedValue() const
{
	// 防止除零错误
	if (FMath::IsNearlyEqual(MaxValue, MinValue))
	{
		return 0.0f;
	}
	
	// 将当前值映射到0-1范围
	return (CurrentValue - MinValue) / (MaxValue - MinValue);
}

/**
 * 通过标准化值（0-1范围）设置当前值
 * 常用于UI滑块控件的回调
 * @param NormalizedValue 0-1范围内的标准化值
 */
void UGameSettingSlider::SetNormalizedValue(float NormalizedValue)
{
	// 确保输入值在0-1范围内
	float ClampedNormalized = FMath::Clamp(NormalizedValue, 0.0f, 1.0f);
	// 将标准化值映射回实际范围
	float NewValue = MinValue + ClampedNormalized * (MaxValue - MinValue);
	SetValue(NewValue);
}

/**
 * 初始化回调
 * 在设置项被初始化时调用
 */
void UGameSettingSlider::OnInitialized()
{
	Super::OnInitialized();
	
	// 存储初始值，用于后续的恢复操作
	StoreInitial();
}

/**
 * 存储初始值
 * 保存当前值作为初始状态，用于RestoreToInitial操作
 */
void UGameSettingSlider::StoreInitial()
{
	Super::StoreInitial();
	InitialValue = CurrentValue;
}

/**
 * 重置为默认值
 * 将当前值恢复为DefaultValue，并触发重置事件
 */
void UGameSettingSlider::ResetToDefault()
{
	Super::ResetToDefault();
	
	// 只有当值确实需要改变时才执行重置
	if (!FMath::IsNearlyEqual(CurrentValue, DefaultValue, 0.001f))
	{
		CurrentValue = DefaultValue;
		NotifySettingChanged(EGameSettingChangeReason::ResetToDefault);
	}
}

/**
 * 恢复到初始值
 * 将当前值恢复为InitialValue（打开设置界面时的值），并触发恢复事件
 */
void UGameSettingSlider::RestoreToInitial()
{
	Super::RestoreToInitial();
	
	// 只有当值确实需要改变时才执行恢复
	if (!FMath::IsNearlyEqual(CurrentValue, InitialValue, 0.001f))
	{
		CurrentValue = InitialValue;
		NotifySettingChanged(EGameSettingChangeReason::RestoreToInitial);
	}
}

/**
 * 将值限制在有效范围内
 * @param Value 要限制的值
 * @return 限制后的值
 */
float UGameSettingSlider::ClampValue(float Value) const
{
	return FMath::Clamp(Value, MinValue, MaxValue);
}

/**
 * 应用步长约束
 * 将值对齐到最近的步长倍数
 * @param Value 要约束的值
 * @return 应用步长约束后的值
 */
float UGameSettingSlider::ApplyStepSize(float Value) const
{
	if (StepSize > 0.0f)
	{
		// 计算从最小值开始的步数，并四舍五入到最近的整数步
		float Steps = FMath::RoundToFloat((Value - MinValue) / StepSize);
		return MinValue + Steps * StepSize;
	}
	return Value;
}

/**
 * 触发设置变更事件
 * 通知系统和UI该设置项的值已发生变化
 * @param Reason 变更原因（用户修改、重置、恢复等）
 */
void UGameSettingSlider::NotifySettingChanged(EGameSettingChangeReason Reason)
{
	// 触发设置变更事件，通知监听者
	OnSettingChanged.Broadcast(this, Reason);
}