#pragma once

#include "CoreMinimal.h"
#include "GameSetting.h"
#include "GameSettingValueScalar.generated.h"

/**
 * 标量值设置项
 * 用于处理数值范围设置，如音量、亮度、灵敏度等
 */
UCLASS(BlueprintType, Blueprintable, meta = (Category = "Game Settings"))
class CANCERGAMESETTING_API UGameSettingValueScalar : public UGameSettingValue
{
	GENERATED_BODY()

public:
	UGameSettingValueScalar();

	// 设置范围
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetMinimumValue(float InMinValue) { MinValue = InMinValue; }

	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetMaximumValue(float InMaxValue) { MaxValue = InMaxValue; }

	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetStepSize(float InStepSize) { StepSize = InStepSize; }

	// 获取/设置当前值
	UFUNCTION(BlueprintCallable, Category = "设置")
	virtual float GetValue() const { return CurrentValue; }

	UFUNCTION(BlueprintCallable, Category = "设置")
	virtual void SetValue(float InValue);

	// 设置默认值
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetDefaultValue(float InDefaultValue) { DefaultValue = InDefaultValue; }

	UFUNCTION(BlueprintCallable, Category = "设置")
	float GetDefaultValue() const { return DefaultValue; }

	// 获取范围信息
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	float GetMinimumValue() const { return MinValue; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	float GetMaximumValue() const { return MaxValue; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	float GetStepSize() const { return StepSize; }

	// 标准化值（0-1范围）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	float GetNormalizedValue() const;

	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetNormalizedValue(float NormalizedValue);

	// 重写基类方法
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;
	virtual FString GetAnalyticsValue() const override;
	virtual void OnApply() override;
	

protected:
	/** 最小值 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	float MinValue = 0.0f;

	/** 最大值 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	float MaxValue = 100.0f;

	/** 步长 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	float StepSize = 1.0f;

	/** 当前值 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	float CurrentValue = 50.0f;

	/** 默认值 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	float DefaultValue = 50.0f;

	/** 初始值（用于恢复） */
	UPROPERTY()
	float InitialValue = 50.0f;

private:
	/** 限制值在有效范围内 */
	float ClampValue(float InValue) const;

	/** 应用步长约束 */
	float ApplyStepSize(float InValue) const;
};