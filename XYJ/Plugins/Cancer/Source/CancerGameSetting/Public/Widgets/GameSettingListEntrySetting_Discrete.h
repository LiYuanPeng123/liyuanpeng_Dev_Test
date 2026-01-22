#pragma once

#include "CoreMinimal.h"
#include "Widgets/GameSettingListEntryBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "GameSettingListEntrySetting_Discrete.generated.h"

class UGameSettingValueDiscrete;

/**
 * 离散选择设置项UI条目
 * 用于显示下拉选择、单选按钮等离散选择类型的设置
 */
UCLASS(Abstract, Blueprintable, meta = (Category = "Settings", DisableNativeTick))
class CANCERGAMESETTING_API UGameSettingListEntrySetting_Discrete : public UGameSettingListEntry_Setting
{
	GENERATED_BODY()
	friend class UGameSettingListView;
public:
	virtual void SetSetting(UGameSetting* InSetting) override;
	void RebindButtonEvents();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnEntryReleased() override;
	virtual void OnSettingChanged() override;

	/**
	 * 处理选项减少（向左）
	 */
	UFUNCTION()
	void HandleOptionDecrease();

	/**
	 * 处理选项增加（向右）
	 */
	UFUNCTION()
	void HandleOptionIncrease();

	/**
	 * 处理下拉框选择变更
	 */
	UFUNCTION()
	void HandleComboBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	/**
	 * 刷新显示
	 */
	void Refresh();

	/**
	 * 蓝图事件：选项变更时调用
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "设置")
	void OnOptionChanged(int32 OptionIndex, const FText& OptionText);

protected:
	/** 离散设置项引用 */
	UPROPERTY()
	TObjectPtr<UGameSettingValueDiscrete> DiscreteSetting;

private:
	// 绑定的UI控件
	/** 值显示面板 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UPanelWidget> Panel_Value;

	/** 当前选项文本 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> Text_SettingValue;

	/** 减少选项按钮 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UButton> Button_Decrease;

	/** 增加选项按钮 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UButton> Button_Increase;

	/** 下拉选择框（可选） */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UComboBoxString> ComboBox_Options;
protected:
	/**
	 * 处理设置项变更事件
	 */
	UFUNCTION()
	void HandleSettingChanged(UGameSetting* ChangedSetting, EGameSettingChangeReason Reason);
public:
	/**
	 * 获取Discrete设置项引用
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	UGameSettingValueDiscrete* GetDiscreteSetting() const { return DiscreteSetting; }
	
	/**
	 * 手动触发选项增加
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void TriggerOptionIncrease() { HandleOptionIncrease(); }
	
	/**
	 * 手动触发选项减少
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void TriggerOptionDecrease() { HandleOptionDecrease(); }
};