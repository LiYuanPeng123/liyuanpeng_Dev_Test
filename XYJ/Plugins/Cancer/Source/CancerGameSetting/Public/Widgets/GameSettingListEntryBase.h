
#pragma once

#include "CoreMinimal.h"
#include "GameSetting.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "GameSettingListEntryBase.generated.h"

class UGameSetting;
class UTextBlock;
class UButton;
class UImage;

/**
 * 设置项UI条目基类
 * 
 * 这是所有设置项UI条目的抽象基类，提供了设置项在列表中显示的基础功能。
 * 主要功能包括：
 * - 设置项数据绑定和显示
 * - 设置变更事件处理
 * - UI状态管理（启用/禁用、可见性等）
 * - 重置和恢复功能
 * - 自定义显示名称支持
 * 
 * 使用方式：
 * 1. 继承此类创建具体的设置项UI
 * 2. 重写相关虚函数实现自定义逻辑
 * 3. 在蓝图中绑定UI控件
 */
UCLASS(Abstract, NotBlueprintable, meta = (Category = "Settings", DisableNativeTick))
class CANCERGAMESETTING_API UGameSettingListEntryBase : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	/**
	 * 设置关联的设置项对象
	 * @param InSetting 要关联的设置项，可以为nullptr来清除关联
	 */
	virtual void SetSetting(UGameSetting* InSetting);

	/**
	 * 设置显示名称覆盖
	 * 允许为设置项指定自定义的显示名称，而不使用默认的DevName
	 * @param OverrideName 自定义显示名称，如果为空则使用默认名称
	 */
	virtual void SetDisplayNameOverride(const FText& OverrideName);

	/**
	 * 获取当前关联的设置项
	 * @return 当前关联的设置项对象，可能为nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取设置项"))
	UGameSetting* GetSetting() const { return Setting; }

	/**
	 * 检查设置项是否可编辑
	 * @return 如果设置项可以被用户编辑则返回true
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "是否可编辑"))
	virtual bool IsSettingEditable() const;

	/**
	 * 检查设置项是否可重置
	 * @return 如果设置项支持重置到默认值则返回true
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "是否可重置"))
	virtual bool IsSettingResetable() const;

	/**
	 * 重置设置项到默认值
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "重置到默认值"))
	virtual void ResetSettingToDefault();

	/**
	 * 恢复设置项到初始值
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "恢复到初始值"))
	virtual void RestoreSettingToInitial();

	/**
	 * 刷新UI显示
	 * 强制更新UI控件的显示状态
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "刷新显示"))
	virtual void RefreshDisplay();

protected:
	/**
	 * 当条目从列表中释放时调用
	 * 重写IUserObjectListEntry接口，用于清理资源和事件绑定
	 */
	virtual void NativeOnEntryReleased() override;

	/**
	 * 当设置项发生变化时调用
	 * 子类应重写此方法来更新UI显示
	 */
	virtual void OnSettingChanged();

	/**
	 * 当编辑条件发生变化时调用
	 * 用于处理设置项的启用/禁用状态变化
	 * @param InSetting 发生变化的设置项
	 */
	virtual void HandleEditConditionChanged(UGameSetting* InSetting);

	/**
	 * 更新UI控件的启用状态
	 * 根据设置项的可编辑状态更新UI控件
	 */
	virtual void UpdateEnabledState();

	/**
	 * 更新显示名称
	 * 根据DisplayNameOverride或设置项的DevName更新显示文本
	 */
	virtual void UpdateDisplayName();

	/**
	 * 更新重置按钮状态
	 * 根据设置项是否可重置来显示/隐藏重置按钮
	 */
	virtual void UpdateResetButtonState();

	/**
	 * 重置按钮点击事件处理
	 */
	UFUNCTION()
	virtual void OnResetButtonClicked();

	/**
	 * 设置项变更事件处理函数
	 * @param InSetting 发生变化的设置项
	 * @param Reason 变化原因
	 */
	void HandleSettingChanged(UGameSetting* InSetting, EGameSettingChangeReason Reason);


	//Item被选中

	UPROPERTY()
	bool bCurIsSelected = false;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void OnItemSelection(bool IsSelection);
	UFUNCTION(BlueprintImplementableEvent)
	void HandleItemSelection(bool IsSelection);

protected:
	/** 当前关联的设置项对象 */
	UPROPERTY()
	TObjectPtr<UGameSetting> Setting;

	/** 自定义显示名称覆盖 */
	FText DisplayNameOverride = FText::GetEmpty();
	
	/** 选中状态材质 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection Materials", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> SelectedMaterial;

	/** 未选中状态材质 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection Materials", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> UnselectedMaterial;
	
	/** 设置项名称文本控件（可选绑定） */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> Text_SettingName;

	/** 选中状态指示器Image控件（可选绑定） */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Selection")
	TObjectPtr<UImage> Image_SelectionIndicator;

	/** 重置按钮控件（可选绑定） */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UButton> Button_Reset;

	/** 设置项图标控件（可选绑定） */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UImage> Image_Icon;

	/** 设置项描述文本控件（可选绑定） */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> Text_Description;

	/** 是否显示重置按钮 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置", meta = (DisplayName = "显示重置按钮"))
	bool bShowResetButton = true;

	/** 是否在设置项禁用时隐藏条目 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置", meta = (DisplayName = "禁用时隐藏"))
	bool bHideWhenDisabled = false;
protected:
	// 新增：鼠标悬停处理
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
/**
 * 通用设置项UI条目类
 * 
 * 这是一个具体的设置项UI实现，提供了标准的设置项显示功能。
 * 包含设置项名称显示和基础的交互功能。
 * 
 * 适用于大多数标准设置项的显示需求。
 */
/**
 * 具体的设置项列表条目实现
 * 继承自UGameSettingListEntryBase，提供基本的设置项显示功能
 */
UCLASS(Abstract, Blueprintable, meta = (Category = "Settings", DisableNativeTick))
class UGameSettingListEntry_Setting : public UGameSettingListEntryBase
{
	GENERATED_BODY()

public:
	/**
	 * 设置要显示的设置项
	 * @param InSetting 要设置的游戏设置项
	 */
	virtual void SetSetting(UGameSetting* InSetting) override;

protected:
	/**
	 * 当设置项发生变化时调用
	 * 重写基类方法，更新设置项名称显示
	 */
	virtual void OnSettingChanged() override;

	// 注意：Text_SettingName 已在基类中定义，无需重复定义
};
