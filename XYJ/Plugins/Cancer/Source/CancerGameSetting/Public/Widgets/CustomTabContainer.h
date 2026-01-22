#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Widgets/CustomTabButton.h"

#include "CustomTabContainer.generated.h"

/**
 * 自定义Tab容器组件
 * 横向布局，支持动态注册Button并提供左右切换功能
 */
UCLASS(BlueprintType, Blueprintable, meta = (Category = "Custom UI"))
class CANCERGAMESETTING_API UCustomTabContainer : public UUserWidget
{
	GENERATED_BODY()

public:
	UCustomTabContainer(const FObjectInitializer& ObjectInitializer);

	// 注册新的Tab按钮
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Container")
	UCustomTabButton* RegisterTabButton(const FText& ButtonText, EGameSettingCategory InCategory,
		TSubclassOf<UCustomTabButton> ButtonClass);

	// 移除Tab按钮
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Container")
	bool RemoveTabButton(UCustomTabButton* ButtonToRemove);

	// 移除所有Tab按钮
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Container")
	void ClearAllTabButtons();

	// 设置选中的Tab按钮
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Container")
	void SetSelectedTab(UCustomTabButton* TabButton);

	// 通过索引设置选中的Tab
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Container")
	void SetSelectedTabByIndex(int32 Index);

	// 获取当前选中的Tab按钮
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Container")
	UCustomTabButton* GetSelectedTab() const { return CurrentSelectedTab; }

	// 获取当前选中Tab的索引
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Container")
	int32 GetSelectedTabIndex() const;

	// 左右切换接口
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Container")
	void NavigateLeft();

	UFUNCTION(BlueprintCallable, Category = "Custom Tab Container")
	void NavigateRight();

	// 获取Tab按钮数量
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Container")
	int32 GetTabButtonCount() const { return TabButtons.Num(); }

	// 通过索引获取Tab按钮
	UFUNCTION(BlueprintCallable, Category = "Custom Tab Container")
	UCustomTabButton* GetTabButtonByIndex(int32 Index) const;

	// Tab按钮点击处理（由CustomTabButton调用）
	UFUNCTION()
	void OnTabButtonClicked(UCustomTabButton* ClickedButton);

protected:
	virtual void NativeConstruct() override;
	// 更新所有按钮的选中状态
	void UpdateButtonSelectionStates();

protected:
	// UI组件绑定
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_TabContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_Tabs;

	// Tab按钮数组
	UPROPERTY()
	TArray<TObjectPtr<UCustomTabButton>> TabButtons;

	// 当前选中的Tab按钮
	UPROPERTY()
	TObjectPtr<UCustomTabButton> CurrentSelectedTab;

	// 默认Tab按钮类
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	TSubclassOf<UCustomTabButton> DefaultTabButtonClass;
	
	// 是否循环导航
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bWrapNavigation = true;

public:
	// Tab选择改变事件委托
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTabSelectionChanged, UCustomTabButton*, NewSelectedTab, int32, TabIndex);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTabSelectionChanged OnTabSelectionChanged;

	// Tab按钮注册事件委托
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTabButtonRegistered, UCustomTabButton*, RegisteredButton, int32, TabIndex);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTabButtonRegistered OnTabButtonRegistered;

	// Tab按钮移除事件委托
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTabButtonRemoved, UCustomTabButton*, RemovedButton, int32, OldTabIndex);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTabButtonRemoved OnTabButtonRemoved;
};