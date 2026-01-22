#pragma once

#include "CoreMinimal.h"
#include "GameSetting.h"
#include "GameSettingSlider.generated.h"

/**
 * 滑块设置项类
 * 继承自UGameSettingValue，用于实现滑块、数值输入等连续数值类型的设置
 */
UCLASS(BlueprintType, meta = (DisplayName = "滑块设置项"))
class CANCERGAMESETTING_API UGameSettingSlider : public UGameSettingValue
{
	GENERATED_BODY()

public:
	UGameSettingSlider();

	// 设置滑块范围和步长
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "设置范围"))
	void SetRange(float InMinValue, float InMaxValue, float InStepSize = 1.0f);

	// 获取/设置当前值
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取当前值"))
	float GetValue() const { return CurrentValue; }

	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "设置当前值"))
	void SetValue(float InValue);

	// 获取范围信息
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取最小值"))
	float GetMinValue() const { return MinValue; }

	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取最大值"))
	float GetMaxValue() const { return MaxValue; }

	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取步长"))
	float GetStepSize() const { return StepSize; }

	// 获取显示文本（可以自定义格式化）
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取显示文本"))
	FText GetDisplayText() const;

	// 设置显示格式（例如："{0}%", "{0}px"等）
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "设置显示格式"))
	void SetDisplayFormat(const FText& Format);

	// 设置默认值
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "设置默认值"))
	void SetDefaultValue(float InDefaultValue);

	// 获取标准化值（0-1范围）
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取标准化值"))
	float GetNormalizedValue() const;

	// 通过标准化值设置（0-1范围）
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "通过标准化值设置"))
	void SetNormalizedValue(float NormalizedValue);

protected:
	// 重写基类方法
	virtual void OnInitialized() override;
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

private:
	// 最小值
	UPROPERTY(EditAnywhere, Category = "设置", meta = (DisplayName = "最小值"))
	float MinValue = 0.0f;

	// 最大值
	UPROPERTY(EditAnywhere, Category = "设置", meta = (DisplayName = "最大值"))
	float MaxValue = 100.0f;

	// 步长
	UPROPERTY(EditAnywhere, Category = "设置", meta = (DisplayName = "步长"))
	float StepSize = 1.0f;

	// 当前值
	UPROPERTY(EditAnywhere, Category = "设置", meta = (DisplayName = "当前值"))
	float CurrentValue = 50.0f;

	// 默认值
	UPROPERTY(EditAnywhere, Category = "设置", meta = (DisplayName = "默认值"))
	float DefaultValue = 50.0f;

	// 显示格式
	UPROPERTY(EditAnywhere, Category = "设置", meta = (DisplayName = "显示格式"))
	FText DisplayFormat = FText::FromString(TEXT("{0}"));

	// 初始值（用于恢复）
	float InitialValue = 50.0f;

	// 限制值在有效范围内
	float ClampValue(float Value) const;

	// 应用步长约束
	float ApplyStepSize(float Value) const;

	// 触发设置变更事件
	void NotifySettingChanged(EGameSettingChangeReason Reason);
};