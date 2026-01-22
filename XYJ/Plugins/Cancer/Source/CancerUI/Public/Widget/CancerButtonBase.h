#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "CancerButtonBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancerButtonDoubleClicked);

/**
 * 自定义按钮基类，仿照UCommonButtonInternalBase设计
 * 提供更精细的按钮控制功能
 */
UCLASS(BlueprintType, Blueprintable)
class CANCERUI_API UCancerButtonBase : public UButton
{
	GENERATED_BODY()

public:
	UCancerButtonBase(const FObjectInitializer& ObjectInitializer);

	/** 设置按钮启用状态 */
	UFUNCTION(BlueprintCallable, Category = "Cancer Button")
	void SetButtonEnabled(bool bInIsButtonEnabled);

	/** 设置按钮交互状态（不影响视觉状态） */
	UFUNCTION(BlueprintCallable, Category = "Cancer Button")
	void SetInteractionEnabled(bool bInIsInteractionEnabled);

	/** 设置按钮是否可获得焦点 */
	UFUNCTION(BlueprintCallable, Category = "Cancer Button")
	void SetButtonFocusable(bool bInIsButtonFocusable);

	/** 检查按钮是否被悬停 */
	bool IsHovered() const;

	/** 检查按钮是否被按下 */
	bool IsPressed() const;

	/** 设置最小期望高度 */
	UFUNCTION(BlueprintCallable, Category = "Cancer Button")
	void SetMinDesiredHeight(int32 InMinHeight);

	/** 设置最小期望宽度 */
	UFUNCTION(BlueprintCallable, Category = "Cancer Button")
	void SetMinDesiredWidth(int32 InMinWidth);

	/** 设置最大期望高度 */
	UFUNCTION(BlueprintCallable, Category = "Cancer Button")
	void SetMaxDesiredHeight(int32 InMaxHeight);

	/** 设置最大期望宽度 */
	UFUNCTION(BlueprintCallable, Category = "Cancer Button")
	void SetMaxDesiredWidth(int32 InMaxWidth);

	/** 双击事件 */
	UPROPERTY(BlueprintAssignable, Category = "Cancer Button|Event")
	FOnCancerButtonDoubleClicked OnDoubleClicked;

public:
	/** 手动触发双击事件（用于测试） */
	UFUNCTION(BlueprintCallable, Category = "Cancer Button")
	void TriggerDoubleClick();

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UWidget interface

	/** Slate 按钮点击事件处理 */
	virtual FReply SlateHandleClicked();

	/** Slate 按钮双击事件处理 */
	virtual FReply SlateHandleDoubleClicked();

	/** Slate 按钮按下事件处理 */
	virtual void SlateHandlePressed();

	/** Slate 按钮释放事件处理 */
	virtual void SlateHandleReleased();

	/** 处理双击事件 */
	virtual void HandleDoubleClicked();

protected:
	/** 按钮的最小宽度 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancer Button")
	int32 MinWidth = 0;

	/** 按钮的最小高度 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancer Button")
	int32 MinHeight = 0;

	/** 按钮的最大宽度 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancer Button")
	int32 MaxWidth = 0;

	/** 按钮的最大高度 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancer Button")
	int32 MaxHeight = 0;

	/** 按钮是否启用 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancer Button")
	bool bButtonEnabled = true;

	/** 按钮是否可交互 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancer Button")
	bool bInteractionEnabled = true;

	/** 按钮是否可获得焦点 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancer Button")
	bool bButtonFocusable = true;

private:
	/** 双击检测相关 */
	double LastClickTime = 0.0;
	static constexpr double DoubleClickTimeThreshold = 0.5; // 双击时间阈值（秒）

	/** 缓存的 SBox 容器 */
	TSharedPtr<class SBox> MyBox;

	/** 缓存的 SButton */
	TSharedPtr<class SButton> MyCancerButton;
};
