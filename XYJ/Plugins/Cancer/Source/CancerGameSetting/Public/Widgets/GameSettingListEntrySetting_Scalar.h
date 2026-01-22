#pragma once

#include "CoreMinimal.h"
#include "Widgets/GameSettingListEntryBase.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "GameSettingListEntrySetting_Scalar.generated.h"

class UGameSettingValueScalar;

/**
 * 数值设置项UI条目
 * 用于显示滑块等数值类型的设置
 */
UCLASS(Abstract, Blueprintable, meta = (Category = "Settings", DisableNativeTick))
class CANCERGAMESETTING_API UGameSettingListEntrySetting_Scalar : public UGameSettingListEntry_Setting
{
	GENERATED_BODY()

	friend class UGameSettingListView;
public:
	virtual void SetSetting(UGameSetting* InSetting) override;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnEntryReleased() override;
	virtual void OnSettingChanged() override;

	/**
	 * 处理滑块值变更
	 */
	UFUNCTION()
	void HandleSliderValueChanged(float Value);

	/**
	 * 处理滑块拖拽结束
	 */
	UFUNCTION()
	void HandleSliderCaptureEnded();

	/**
	 * 刷新显示
	 */
	void Refresh();

	/**
	 * 蓝图事件：值变更时调用
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "设置")
	void OnValueChanged(float Value);

	/**
	 * 蓝图事件：默认值变更时调用
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "设置")
	void OnDefaultValueChanged(float DefaultValue);

protected:
	/** 数值设置项引用 */
	UPROPERTY()
	TObjectPtr<UGameSettingValueScalar> ScalarSetting;

private:
	// 绑定的UI控件
	/** 值显示面板 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UPanelWidget> Panel_Value;

	/** 数值滑块 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<USlider> Slider_SettingValue;

	/** 数值文本显示 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> Text_SettingValue;

public:
	/**
	 * 获取Scalar设置项引用
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	UGameSettingValueScalar* GetScalarSetting() const { return ScalarSetting; }
	
	/**
	 * 手动设置滑条值
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetSliderValue(float Value);
	
	/**
	 * 手动调整滑条值（增量）
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void AdjustSliderValue(float DeltaValue);
};