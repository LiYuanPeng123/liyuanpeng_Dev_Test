#pragma once

#include "CoreMinimal.h"
#include "Widgets/GameSettingListEntryBase.h"
#include "Components/Button.h"
#include "GameSettingListEntrySetting_Action.generated.h"

class UGameSettingAction;

/**
 * 动作设置项UI条目
 * 用于显示按钮等动作类型的设置
 */
UCLASS(Abstract, Blueprintable, meta = (Category = "Settings", DisableNativeTick))
class CANCERGAMESETTING_API UGameSettingListEntrySetting_Action : public UGameSettingListEntry_Setting
{
	GENERATED_BODY()

public:
	virtual void SetSetting(UGameSetting* InSetting) override;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnEntryReleased() override;

	/**
	 * 处理动作按钮点击
	 */
	UFUNCTION()
	void HandleActionButtonClicked();

	/**
	 * 蓝图事件：设置分配时调用
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "设置")
	void OnSettingAssigned(const FText& ActionText);

protected:
	/** 动作设置项引用 */
	UPROPERTY()
	TObjectPtr<UGameSettingAction> ActionSetting;

private:
	// 绑定的UI控件
	/** 动作按钮 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UButton> Button_Action;
};