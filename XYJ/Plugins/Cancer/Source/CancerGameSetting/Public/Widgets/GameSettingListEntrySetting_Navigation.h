#pragma once

#include "CoreMinimal.h"
#include "Widgets/GameSettingListEntryBase.h"
#include "Components/Button.h"
#include "GameSettingListEntrySetting_Navigation.generated.h"

class UGameSettingCollection;

/**
 * 导航设置项UI条目
 * 用于显示导航到子页面的设置项
 */
UCLASS(Abstract, Blueprintable, meta = (Category = "Settings", DisableNativeTick))
class CANCERGAMESETTING_API UGameSettingListEntrySetting_Navigation : public UGameSettingListEntry_Setting
{
	GENERATED_BODY()

public:
	virtual void SetSetting(UGameSetting* InSetting) override;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnEntryReleased() override;

	/**
	 * 处理导航按钮点击
	 */
	UFUNCTION()
	void HandleNavigationButtonClicked();

	/**
	 * 蓝图事件：设置分配时调用
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "设置")
	void OnSettingAssigned(const FText& ActionText);

	/**
	 * 蓝图事件：导航请求
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "设置")
	void OnNavigationRequested(UGameSettingCollection* TargetCollection);

protected:
	/** 集合设置项引用 */
	UPROPERTY()
	TObjectPtr<UGameSettingCollection> CollectionSetting;

private:
	// 绑定的UI控件
	/** 导航按钮 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UButton> Button_Navigate;
};