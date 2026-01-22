#pragma once

#include "CoreMinimal.h"
#include "GameSetting.h"
#include "GameSettingTextOption.generated.h"

/**
 * 文本选择设置项类
 * 继承自UGameSettingValueDiscrete，用于实现下拉选择、单选按钮等离散选择类型的设置
 */
UCLASS(BlueprintType, meta = (DisplayName = "文本选择设置项"))
class CANCERGAMESETTING_API UGameSettingTextOption : public UGameSettingValueDiscrete
{
	GENERATED_BODY()

public:
	UGameSettingTextOption();

	// 设置选项列表
	virtual void SetOptions(const TArray<FText>& InOptions) override ;

	// 添加单个选项
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "添加选项"))
	void AddOption(const FText& Option);

	// 清空所有选项
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "清空选项"))
	void ClearOptions();

	// 重写基类方法 - 通过索引设置选中项
	virtual void SetDiscreteOptionByIndex(int32 Index) override;

	// 重写基类方法 - 获取当前选中项索引
	virtual int32 GetDiscreteOptionIndex() const override;

	// 重写基类方法 - 获取默认选中项索引
	virtual int32 GetDiscreteOptionDefaultIndex() const override;

	// 重写基类方法 - 获取所有选项
	virtual TArray<FText> GetDiscreteOptions() const override;

	// 获取当前选中的文本
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取选中文本"))
	FText GetSelectedText() const;

	// 通过文本设置选中项
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "通过文本设置选中项"))
	bool SetSelectedByText(const FText& Text);

	// 设置默认选中索引
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "设置默认索引"))
	void SetDefaultIndex(int32 Index);

protected:
	// 重写基类方法
	virtual void OnInitialized() override;
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

private:

	// 初始索引（用于恢复）
	int32 InitialIndex = 0;

	// 验证索引是否有效
	bool IsValidIndex(int32 Index) const;

	// 触发设置变更事件
	void NotifySettingChanged(EGameSettingChangeReason Reason);
};