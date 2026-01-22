#include "Widgets/GameSettingListEntrySetting_Scalar.h"
#include "GameSettingValueScalar.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

void UGameSettingListEntrySetting_Scalar::SetSetting(UGameSetting* InSetting)
{
	ScalarSetting = Cast<UGameSettingValueScalar>(InSetting);
	Super::SetSetting(InSetting);

	//重新绑定滑块事件
	if (Slider_SettingValue)
	{
		Slider_SettingValue->OnValueChanged.RemoveAll(this);
		Slider_SettingValue->OnMouseCaptureEnd.RemoveAll(this);
		Slider_SettingValue->OnValueChanged.AddDynamic(this, &UGameSettingListEntrySetting_Scalar::HandleSliderValueChanged);
		Slider_SettingValue->OnMouseCaptureEnd.AddDynamic(this, &UGameSettingListEntrySetting_Scalar::HandleSliderCaptureEnded);
	}
	
	Refresh();
}

void UGameSettingListEntrySetting_Scalar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 绑定滑块事件
	if (Slider_SettingValue)
	{
		Slider_SettingValue->OnValueChanged.AddDynamic(this, &UGameSettingListEntrySetting_Scalar::HandleSliderValueChanged);
		Slider_SettingValue->OnMouseCaptureEnd.AddDynamic(this, &UGameSettingListEntrySetting_Scalar::HandleSliderCaptureEnded);
	}
}

void UGameSettingListEntrySetting_Scalar::NativeOnEntryReleased()
{
	// 清理事件绑定
	if (Slider_SettingValue)
	{
		Slider_SettingValue->OnValueChanged.RemoveAll(this);
		Slider_SettingValue->OnMouseCaptureEnd.RemoveAll(this);
	}

	Super::NativeOnEntryReleased();
}

void UGameSettingListEntrySetting_Scalar::OnSettingChanged()
{
	Super::OnSettingChanged();
	Refresh();
}

void UGameSettingListEntrySetting_Scalar::HandleSliderValueChanged(float Value)
{
	if (ScalarSetting)
	{
		// 将滑块的0-1值转换为实际值
		float ActualValue = ScalarSetting->GetMinimumValue() + 
			Value * (ScalarSetting->GetMaximumValue() - ScalarSetting->GetMinimumValue());
		
		ScalarSetting->SetValue(ActualValue);
		
		// 更新文本显示
		if (Text_SettingValue)
		{
			Text_SettingValue->SetText(FText::AsNumber(ActualValue));
		}
		
		// 触发蓝图事件
		OnValueChanged(Value);
	}
}

void UGameSettingListEntrySetting_Scalar::HandleSliderCaptureEnded()
{
	// 滑块拖拽结束，可以在这里执行额外的逻辑
	if (ScalarSetting)
	{
		// 例如：应用设置或保存设置
		ScalarSetting->Apply();
	}
}

void UGameSettingListEntrySetting_Scalar::Refresh()
{
	if (!ScalarSetting)
	{
		return;
	}

	float CurrentValue = ScalarSetting->GetValue();
	float MinValue = ScalarSetting->GetMinimumValue();
	float MaxValue = ScalarSetting->GetMaximumValue();

	// 更新滑块
	float NormalizedValue = (CurrentValue - MinValue) / (MaxValue - MinValue);
	if (Slider_SettingValue)
	{
		// 将实际值转换为0-1范围

		Slider_SettingValue->SetValue(NormalizedValue);
	}

	// 更新文本显示
	if (Text_SettingValue)
	{
		Text_SettingValue->SetText(FText::AsNumber(CurrentValue));
	}

	// 触发蓝图事件
	OnValueChanged(NormalizedValue);
	float DefaultValueValue = (ScalarSetting->GetDefaultValue() - MinValue) / (MaxValue - MinValue);
	OnDefaultValueChanged(DefaultValueValue);
}

void UGameSettingListEntrySetting_Scalar::SetSliderValue(float Value)
{
	if (ScalarSetting && Slider_SettingValue)
	{
		// 直接设置滑条值（0-1范围）
		Slider_SettingValue->SetValue(Value);
		
		// 触发值变更处理
		HandleSliderValueChanged(Value);
	}
}

void UGameSettingListEntrySetting_Scalar::AdjustSliderValue(float DeltaValue)
{
	if (ScalarSetting)
	{
		float CurrentValue = ScalarSetting->GetValue();
		float NewValue = CurrentValue + DeltaValue;
		
		// 限制在有效范围内
		float MinValue = ScalarSetting->GetMinimumValue();
		float MaxValue = ScalarSetting->GetMaximumValue();
		NewValue = FMath::Clamp(NewValue, MinValue, MaxValue);
		
		// 转换为滑条的0-1值
		float NormalizedValue = (NewValue - MinValue) / (MaxValue - MinValue);
		
		// 设置滑条值
		SetSliderValue(NormalizedValue);
	}
}