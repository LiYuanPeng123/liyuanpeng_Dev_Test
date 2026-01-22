#pragma once

#include "CoreMinimal.h"
#include "GameSetting.h"
#include "Engine/Engine.h"
#include "GameSettingAction.generated.h"

// 声明C++委托类型（不暴露给蓝图）
DECLARE_DELEGATE(FGameSettingActionDelegate);

/**
 * 动作设置项
 * 用于执行特定操作的设置项，如重置、应用等按钮功能
 */
UCLASS(BlueprintType, Blueprintable, meta = (Category = "Game Settings"))
class CANCERGAMESETTING_API UGameSettingAction : public UGameSetting
{
	GENERATED_BODY()

public:
	UGameSettingAction();

	/**
	 * 执行动作
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	virtual void ExecuteAction();

	/**
	 * 检查动作是否可执行
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	virtual bool CanExecuteAction() const;

	/**
	 * 设置动作委托（C++使用）
	 */
	void SetActionDelegate(const FGameSettingActionDelegate& InDelegate);

	/**
	 * 应用设置
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void Apply();

	/**
	 * 设置动作按钮文本
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetActionButtonText(const FText& InButtonText);

	/**
	 * 获取动作按钮文本
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	FText GetActionButtonText() const { return ActionButtonText; }

	/**
	 * 设置确认文本
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetConfirmationText(const FText& InConfirmationText);

	/**
	 * 获取确认文本
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	FText GetConfirmationText() const { return ConfirmationText; }

	/**
	 * 设置是否需要确认
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetRequiresConfirmation(bool bInRequiresConfirmation);

	/**
	 * 获取是否需要确认
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	bool GetRequiresConfirmation() const { return bRequiresConfirmation; }

protected:
	/**
	 * 应用设置回调
	 * 重写基类方法
	 */
	virtual void OnApply() override;

	/**
	 * 动作执行委托（C++内部使用）
	 */
	FGameSettingActionDelegate ActionDelegate;

	/**
	 * 是否启用动作
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	bool bActionEnabled = true;

	/**
	 * 动作按钮显示文本
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	FText ActionButtonText;

	/**
	 * 确认对话框文本
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	FText ConfirmationText;

	/**
	 * 是否需要确认对话框
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	bool bRequiresConfirmation = false;

public:
	// 蓝图事件
	/** 动作执行前事件 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionExecuting, UGameSettingAction*, Action);
	UPROPERTY(BlueprintAssignable, Category = "设置事件")
	FOnActionExecuting OnActionExecuting;

	/** 动作执行后事件 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionExecuted, UGameSettingAction*, Action);
	UPROPERTY(BlueprintAssignable, Category = "设置事件")
	FOnActionExecuted OnActionExecuted;

protected:
	/**
	 * 蓝图可重写的动作执行函数
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "设置", meta = (DisplayName = "执行动作"))
	void ReceiveExecuteAction();

	/**
	 * 蓝图可重写的动作可执行检查函数
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "设置", meta = (DisplayName = "检查是否可执行"))
	bool ReceiveCanExecuteAction() const;
};