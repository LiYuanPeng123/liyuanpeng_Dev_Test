#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Materials/MaterialInterface.h"
#include "Widgets/GameSettingMainWidget.h"
#include "CustomTabButton.generated.h"

// 前向声明
class UCustomTabContainer;

/**
 * 自定义Tab按钮组件
 * 支持悬停、点击、选中等状态的视觉效果
 * 集成EGameSettingCategory支持
 */
UCLASS(BlueprintType, Blueprintable)
class CANCERGAMESETTING_API UCustomTabButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UCustomTabButton(const FObjectInitializer& ObjectInitializer);

	// 按钮点击事件委托
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabButtonClicked, UCustomTabButton*, ClickedButton);
	UPROPERTY(BlueprintAssignable, Category = "Tab Events")
	FOnTabButtonClicked OnTabButtonClicked;

	/**
	 * 初始化按钮
	 * @param InText 按钮显示文本
	 * @param InCategory 关联的游戏设置分类
	 * @param InContainer 父容器引用
	 */
	UFUNCTION(BlueprintCallable, Category = "Tab Button")
	void InitializeButton(const FText& InText, EGameSettingCategory InCategory, UCustomTabContainer* InContainer);

	/**
	 * 设置按钮文本
	 */
	UFUNCTION(BlueprintCallable, Category = "Tab Button")
	void SetButtonText(const FText& InText);

	/**
	 * 获取按钮文本
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab Button")
	FText GetButtonText() const;

	/**
	 * 获取关联的游戏设置分类
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab Button")
	EGameSettingCategory GetCategory() const { return Category; }

	/**
	 * 设置按钮选中状态
	 */
	UFUNCTION(BlueprintCallable, Category = "Tab Button")
	void SetSelected(bool bInSelected);

	/**
	 * 获取按钮选中状态
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab Button")
	bool IsSelected() const { return bIsSelected; }

	/**
	 * 设置按钮启用状态
	 */
	UFUNCTION(BlueprintCallable, Category = "Tab Button")
	void SetButtonEnabled(bool bInEnabled);

	/**
	 * 获取按钮启用状态
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab Button")
	bool IsButtonEnabled() const { return bIsEnabled; }

	// 设置文本颜色
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Button")
	void SetTextColor(FSlateColor InColor);
	// 设置悬停文本颜色
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Button")
	void SetHoveredTextColor(FSlateColor InColor);
	// 设置选中文本颜色
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Button")
	void SetSelectedTextColor(FSlateColor InColor);
	// 设置禁用文本颜色
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Button")
	void SetDisabledTextColor(FSlateColor InColor);
	// 应用文本颜色
	void ApplyTextColor(const FSlateColor& InColor);

	// 材质设置方法
	UFUNCTION(BlueprintCallable, Category = "Tab Button")
	void SetNormalMaterial(UMaterialInterface* InMaterial);

	UFUNCTION(BlueprintCallable, Category = "Tab Button")
	void SetHoveredMaterial(UMaterialInterface* InMaterial);

	UFUNCTION(BlueprintCallable, Category = "Tab Button")
	void SetPressedMaterial(UMaterialInterface* InMaterial);

	UFUNCTION(BlueprintCallable, Category = "Tab Button")
	void SetSelectedMaterial(UMaterialInterface* InMaterial);

	UFUNCTION(BlueprintCallable, Category = "Tab Button")
	void SetDisabledMaterial(UMaterialInterface* InMaterial);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 按钮事件处理
	UFUNCTION()
	void OnButtonClicked();

	UFUNCTION()
	void OnButtonHovered();

	UFUNCTION()
	void OnButtonUnhovered();

	UFUNCTION()
	void OnButtonPressed();

	UFUNCTION()
	void OnButtonReleased();

	// 更新视觉状态
	void UpdateVisualState();

	// UI组件绑定
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonText;

private:
	// 按钮状态
	bool bIsSelected = false;
	bool bIsHovered = false;
	bool bIsPressed = false;
	bool bIsEnabled = true;

	// 关联的游戏设置分类
	UPROPERTY()
	EGameSettingCategory Category = EGameSettingCategory::Graphics;

	// 父容器引用
	UPROPERTY()
	TObjectPtr<UCustomTabContainer> ParentContainer;

	// 材质资源
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> NormalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> HoveredMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> PressedMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> SelectedMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> DisabledMaterial;

	// 文本颜色属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	FSlateColor NormalTextColor = FSlateColor(FLinearColor::White);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	FSlateColor HoveredTextColor = FSlateColor(FLinearColor::Yellow);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	FSlateColor SelectedTextColor = FSlateColor(FLinearColor::Green);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	FSlateColor PressedTextColor = FSlateColor(FLinearColor::Yellow);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance", meta = (AllowPrivateAccess = "true"))
	FSlateColor DisabledTextColor = FSlateColor(FLinearColor::Gray);
};