#include "Widgets/GameSettingListEntrySetting_Discrete.h"
#include "GameSetting.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Components/PanelWidget.h"

void UGameSettingListEntrySetting_Discrete::SetSetting(UGameSetting* InSetting)
{
	// 清理旧的事件绑定
	if (DiscreteSetting)
	{
		DiscreteSetting->OnSettingChanged.RemoveAll(this);
	}

	DiscreteSetting = Cast<UGameSettingValueDiscrete>(InSetting);
	Super::SetSetting(InSetting);
	
	// 重新绑定事件
	if (DiscreteSetting)
	{
		DiscreteSetting->OnSettingChanged.AddUObject(this, &UGameSettingListEntrySetting_Discrete::HandleSettingChanged);
	}
	
	// 重新绑定按钮事件
	RebindButtonEvents();
	
	// 确保名称显示正确
	if (Setting && Text_SettingName)
	{
		Text_SettingName->SetText(Setting->GetDisplayName());
	}
	
	Refresh();
}

// 添加新方法：重新绑定按钮事件
void UGameSettingListEntrySetting_Discrete::RebindButtonEvents()
{
	// 先清理可能存在的绑定
	if (Button_Decrease)
	{
		Button_Decrease->OnClicked.RemoveAll(this);
		Button_Decrease->OnClicked.AddDynamic(this, &UGameSettingListEntrySetting_Discrete::HandleOptionDecrease);
	}

	if (Button_Increase)
	{
		Button_Increase->OnClicked.RemoveAll(this);
		Button_Increase->OnClicked.AddDynamic(this, &UGameSettingListEntrySetting_Discrete::HandleOptionIncrease);
	}

	if (ComboBox_Options)
	{
		ComboBox_Options->OnSelectionChanged.RemoveAll(this);
		ComboBox_Options->OnSelectionChanged.AddDynamic(this, &UGameSettingListEntrySetting_Discrete::HandleComboBoxSelectionChanged);
	}
}

void UGameSettingListEntrySetting_Discrete::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 绑定按钮事件
	if (Button_Decrease)
	{
		Button_Decrease->OnClicked.AddDynamic(this, &UGameSettingListEntrySetting_Discrete::HandleOptionDecrease);
	}

	if (Button_Increase)
	{
		Button_Increase->OnClicked.AddDynamic(this, &UGameSettingListEntrySetting_Discrete::HandleOptionIncrease);
	}

	if (ComboBox_Options)
	{
		ComboBox_Options->OnSelectionChanged.AddDynamic(this, &UGameSettingListEntrySetting_Discrete::HandleComboBoxSelectionChanged);
	}
}

void UGameSettingListEntrySetting_Discrete::NativeOnEntryReleased()
{
	// 清理所有事件绑定
	if (DiscreteSetting)
	{
		DiscreteSetting->OnSettingChanged.RemoveAll(this);
	}

	if (Button_Decrease)
	{
		Button_Decrease->OnClicked.RemoveAll(this);
	}

	if (Button_Increase)
	{
		Button_Increase->OnClicked.RemoveAll(this);
	}

	if (ComboBox_Options)
	{
		ComboBox_Options->OnSelectionChanged.RemoveAll(this);
	}

	Super::NativeOnEntryReleased();
}

void UGameSettingListEntrySetting_Discrete::OnSettingChanged()
{
	Super::OnSettingChanged();
	Refresh();
}

void UGameSettingListEntrySetting_Discrete::HandleOptionDecrease()
{
	if (!DiscreteSetting)
	{
		UE_LOG(LogTemp, Error, TEXT("减少按钮: DiscreteSetting 为空"));
		return;
	}

	TArray<FText> Options = DiscreteSetting->GetDiscreteOptions();
	int32 CurrentIndex = DiscreteSetting->GetDiscreteOptionIndex();
	int32 NewIndex = FMath::Max(0, CurrentIndex - 1);
	
	UE_LOG(LogTemp, Warning, TEXT("=== 减少按钮点击 ==="));
	UE_LOG(LogTemp, Warning, TEXT("当前: [%d]='%s'"), 
		CurrentIndex, 
		Options.IsValidIndex(CurrentIndex) ? *Options[CurrentIndex].ToString() : TEXT("无效索引"));
	UE_LOG(LogTemp, Warning, TEXT("目标: [%d]='%s'"), 
		NewIndex,
		Options.IsValidIndex(NewIndex) ? *Options[NewIndex].ToString() : TEXT("无效索引"));
	
	if (NewIndex != CurrentIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("执行索引变更: %d -> %d"), CurrentIndex, NewIndex);
		DiscreteSetting->SetDiscreteOptionByIndex(NewIndex);
		
		// 立即刷新
		Refresh();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("索引未变更，已经是最小值"));
	}
}

void UGameSettingListEntrySetting_Discrete::HandleOptionIncrease()
{
	if (!DiscreteSetting)
	{
		UE_LOG(LogTemp, Error, TEXT("增加按钮: DiscreteSetting 为空"));
		return;
	}

	TArray<FText> Options = DiscreteSetting->GetDiscreteOptions();
	int32 CurrentIndex = DiscreteSetting->GetDiscreteOptionIndex();
	int32 NewIndex = FMath::Min(Options.Num() - 1, CurrentIndex + 1);
	
	UE_LOG(LogTemp, Warning, TEXT("=== 增加按钮点击 ==="));
	UE_LOG(LogTemp, Warning, TEXT("当前: [%d]='%s'"), 
		CurrentIndex, 
		Options.IsValidIndex(CurrentIndex) ? *Options[CurrentIndex].ToString() : TEXT("无效索引"));
	UE_LOG(LogTemp, Warning, TEXT("目标: [%d]='%s'"), 
		NewIndex,
		Options.IsValidIndex(NewIndex) ? *Options[NewIndex].ToString() : TEXT("无效索引"));
	
	if (NewIndex != CurrentIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("执行索引变更: %d -> %d"), CurrentIndex, NewIndex);
		DiscreteSetting->SetDiscreteOptionByIndex(NewIndex);
		
		// 立即刷新
		Refresh();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("索引未变更，已经是最大值"));
	}
}

void UGameSettingListEntrySetting_Discrete::HandleComboBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (DiscreteSetting && ComboBox_Options && SelectionType == ESelectInfo::OnMouseClick)
	{
		int32 SelectedIndex = ComboBox_Options->GetSelectedIndex();
		DiscreteSetting->SetDiscreteOptionByIndex(SelectedIndex);
	}
}

void UGameSettingListEntrySetting_Discrete::HandleSettingChanged(UGameSetting* ChangedSetting, EGameSettingChangeReason Reason)
{
	if (ChangedSetting == DiscreteSetting)
	{
		// 根据变更原因执行不同的处理
		switch (Reason)
		{
		case EGameSettingChangeReason::ResetToDefault:
		case EGameSettingChangeReason::RestoreToInitial:
			// 重置或恢复操作，需要完全刷新
			Refresh();
			break;
		case EGameSettingChangeReason::Change:
			// 普通变更，只更新显示
			Refresh();
			break;
		default:
			break;
		}
	}
}

void UGameSettingListEntrySetting_Discrete::Refresh()
{
	// 尝试重新获取 DiscreteSetting 引用
	if (!DiscreteSetting && Setting)
	{
		DiscreteSetting = Cast<UGameSettingValueDiscrete>(Setting);
	}

	if (!DiscreteSetting)
	{
		UE_LOG(LogTemp, Warning, TEXT("DiscreteSetting is null in Refresh, Setting=%s"), 
			Setting ? *Setting->GetClass()->GetName() : TEXT("NULL"));
		
		// 禁用所有控件
		if (Button_Decrease) Button_Decrease->SetIsEnabled(false);
		if (Button_Increase) Button_Increase->SetIsEnabled(false);
		if (Text_SettingValue) Text_SettingValue->SetText(FText::FromString(TEXT("无效设置")));
		return;
	}

	TArray<FText> Options = DiscreteSetting->GetDiscreteOptions();
	int32 CurrentIndex = DiscreteSetting->GetDiscreteOptionIndex();

	// 添加详细的调试信息
	UE_LOG(LogTemp, Warning, TEXT("=== 图形质量设置调试 ==="));
	UE_LOG(LogTemp, Warning, TEXT("当前索引: %d"), CurrentIndex);
	UE_LOG(LogTemp, Warning, TEXT("选项总数: %d"), Options.Num());
	
	for (int32 i = 0; i < Options.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("选项[%d]: %s %s"), 
			i, 
			*Options[i].ToString(),
			(i == CurrentIndex) ? TEXT("← 当前") : TEXT(""));
	}

	// 更新按钮状态
	bool bCanDecrease = CurrentIndex > 0;
	bool bCanIncrease = CurrentIndex < Options.Num() - 1;
	
	UE_LOG(LogTemp, Warning, TEXT("减少按钮可用: %s (条件: %d > 0)"), 
		bCanDecrease ? TEXT("是") : TEXT("否"), CurrentIndex);
	UE_LOG(LogTemp, Warning, TEXT("增加按钮可用: %s (条件: %d < %d)"), 
		bCanIncrease ? TEXT("是") : TEXT("否"), CurrentIndex, Options.Num() - 1);

	if (Button_Decrease)
	{
		Button_Decrease->SetIsEnabled(bCanDecrease);
		UE_LOG(LogTemp, Warning, TEXT("减少按钮状态设置为: %s"), bCanDecrease ? TEXT("启用") : TEXT("禁用"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("减少按钮为空！"));
	}

	if (Button_Increase)
	{
		Button_Increase->SetIsEnabled(bCanIncrease);
		UE_LOG(LogTemp, Warning, TEXT("增加按钮状态设置为: %s"), bCanIncrease ? TEXT("启用") : TEXT("禁用"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("增加按钮为空！"));
	}

	UE_LOG(LogTemp, Log, TEXT("Refresh: CurrentIndex=%d, Options.Num()=%d, Setting=%s"), 
		CurrentIndex, Options.Num(), *DiscreteSetting->GetDevName().ToString());

	// 验证索引有效性
	if (!Options.IsValidIndex(CurrentIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid CurrentIndex %d for %d options"), CurrentIndex, Options.Num());
		CurrentIndex = 0;
		if (Options.Num() > 0)
		{
			DiscreteSetting->SetDiscreteOptionByIndex(0);
		}
	}

	// 更新文本显示
	if (Text_SettingValue)
	{
		if (Options.IsValidIndex(CurrentIndex))
		{
			Text_SettingValue->SetText(Options[CurrentIndex]);
			UE_LOG(LogTemp, Log, TEXT("Updated Text_SettingValue to: %s"), *Options[CurrentIndex].ToString());
		}
		else
		{
			Text_SettingValue->SetText(FText::FromString(TEXT("无选项")));
		}
	}
	

	// 更新下拉框
	if (ComboBox_Options)
	{
		ComboBox_Options->ClearOptions();
		for (const FText& Option : Options)
		{
			ComboBox_Options->AddOption(Option.ToString());
		}
		if (Options.IsValidIndex(CurrentIndex))
		{
			ComboBox_Options->SetSelectedIndex(CurrentIndex);
		}
	}

	// 触发蓝图事件
	if (Options.IsValidIndex(CurrentIndex))
	{
		OnOptionChanged(CurrentIndex, Options[CurrentIndex]);
	}
}