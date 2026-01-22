#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "GameSetting.h"
#include "GameSettingCollection.h"
#include "GameSettingListView.h"
#include "GameSettingMainWidget.generated.h"

class UGameSettingCollection;
class UGameSettingListView;
class UButton;
class UTextBlock;
class UScrollBox;
class UWidgetSwitcher;

/**
 * 游戏设置分类枚举
 */
UENUM(BlueprintType)
enum class EGameSettingCategory : uint8
{
	Graphics		UMETA(DisplayName = "图形设置"),
	Audio			UMETA(DisplayName = "音频设置"),
	Controls		UMETA(DisplayName = "控制设置"),
	Gameplay		UMETA(DisplayName = "游戏性设置"),
	Accessibility	UMETA(DisplayName = "辅助功能"),
	Advanced		UMETA(DisplayName = "高级设置")
};

/**
 * 游戏设置主控件
 * 提供完整的游戏设置界面，包含分类导航、设置项显示、保存/重置等功能
 */
UCLASS(BlueprintType, Blueprintable, meta = (Category = "Game Settings"))
class CANCERGAMESETTING_API UGameSettingMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGameSettingMainWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	/**
	 * 初始化设置界面
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void InitializeSettings();

	/**
	 * 切换到指定设置分类
	 * @param Category 要切换到的设置分类
	 * @param bForceRefresh 是否强制刷新，即使分类相同也会更新UI
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SwitchToCategory(EGameSettingCategory Category, bool bForceRefresh = false);

	/**
	 * 保存所有设置
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SaveAllSettings();

	/**
	 * 重置当前分类的所有设置到默认值
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void ResetCurrentCategoryToDefaults();

	/**
	 * 重置所有设置到默认值
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void ResetAllSettingsToDefaults();

	/**
	 * 应用设置更改
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void ApplySettings();

	/**
	 * 取消设置更改，恢复到上次保存的状态
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void CancelChanges();

	/**
	 * 检查是否有未保存的更改
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	bool HasUnsavedChanges() const;

	/**
	 * 获取当前选中的设置分类
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	EGameSettingCategory GetCurrentCategory() const { return CurrentCategory; }

	/**
 * 设置项变化回调
 */
	UFUNCTION()
	void OnSettingChanged(UGameSetting* Setting, EGameSettingChangeReason Reason);
protected:
	/**
	 * 更新设置列表显示
	 */
	void UpdateSettingsList();

	/**
	 * 绑定设置项事件
	 */
	void BindSettingEvents(UGameSettingCollection* Collection);

	/**
	 * 解绑所有设置项事件
	 */
	void UnbindAllSettingEvents();

	/**
	 * 分类按钮点击事件
	 */
	UFUNCTION()
	void OnCategoryButtonClicked(EGameSettingCategory Category);

	/**
	 * 保存按钮点击事件
	 */
	UFUNCTION()
	void OnSaveButtonClicked();

	/**
	 * 重置按钮点击事件
	 */
	UFUNCTION()
	void OnResetButtonClicked();

	/**
	 * 应用按钮点击事件
	 */
	UFUNCTION()
	void OnApplyButtonClicked();

	/**
	 * 取消按钮点击事件
	 */
	UFUNCTION()
	void OnCancelButtonClicked();



protected:
	/** 当前选中的设置分类 */
	UPROPERTY(BlueprintReadOnly, Category = "设置")
	EGameSettingCategory CurrentCategory = EGameSettingCategory::Graphics;

	/** 设置分类集合映射 */
	UPROPERTY()
	TMap<EGameSettingCategory, TObjectPtr<UGameSettingCollection>> CategoryCollections;

	/** 是否有未保存的更改 */
	UPROPERTY(BlueprintReadOnly, Category = "设置")
	bool bHasUnsavedChanges = false;

	// UI 控件绑定
	/** 分类导航容器 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> CategoryNavigation;
	

	/** 设置列表视图 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UGameSettingListView> SettingListView;

	/** 底部按钮容器 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> BottomButtonContainer;

	/** 保存按钮 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Button_Save;

	/** 重置按钮 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Button_Reset;

	/** 应用按钮 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Button_Apply;

	/** 取消按钮 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Button_Cancel;
	
	// 设置项配置
	/** 是否启用自动保存 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置配置")
	bool bAutoSave = true;

	/** 是否显示高级设置 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置配置")
	bool bShowAdvancedSettings = false;

	/** 是否启用设置搜索 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置配置")
	bool bEnableSettingSearch = true;

public:
	// 蓝图事件
	/** 设置保存完成事件 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsSaved);
	UPROPERTY(BlueprintAssignable, Category = "设置事件")
	FOnSettingsSaved OnSettingsSaved;

	/** 设置重置完成事件 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsReset);
	UPROPERTY(BlueprintAssignable, Category = "设置事件")
	FOnSettingsReset OnSettingsReset;

	/** 设置分类切换事件 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategorySwitched, EGameSettingCategory, NewCategory);
	UPROPERTY(BlueprintAssignable, Category = "设置事件")
	FOnCategorySwitched OnCategorySwitched;

	/** 设置更改事件 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSettingChanged, UGameSetting*, Setting, bool, bHasUnsavedChanges);
	UPROPERTY(BlueprintAssignable, Category = "设置事件")
	FOnSettingChanged OnSettingChangedEvent;
};