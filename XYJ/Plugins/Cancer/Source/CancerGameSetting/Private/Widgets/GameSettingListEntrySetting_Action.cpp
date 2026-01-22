#include "Widgets/GameSettingListEntrySetting_Action.h"
#include "GameSettingAction.h"
#include "Components/Button.h"

void UGameSettingListEntrySetting_Action::SetSetting(UGameSetting* InSetting)
{
	ActionSetting = Cast<UGameSettingAction>(InSetting);
	Super::SetSetting(InSetting);

	if (ActionSetting && Button_Action)
	{
		// 设置按钮文本
		FText ActionText = ActionSetting->GetActionButtonText();
		if (ActionText.IsEmpty())
		{
			ActionText = ActionSetting->GetDisplayName();
		}
		
		// 触发蓝图事件
		OnSettingAssigned(ActionText);
	}
}

void UGameSettingListEntrySetting_Action::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 绑定按钮事件
	if (Button_Action)
	{
		Button_Action->OnClicked.AddDynamic(this, &UGameSettingListEntrySetting_Action::HandleActionButtonClicked);
	}
}

void UGameSettingListEntrySetting_Action::NativeOnEntryReleased()
{
	// 清理事件绑定
	if (Button_Action)
	{
		Button_Action->OnClicked.RemoveAll(this);
	}

	Super::NativeOnEntryReleased();
}

void UGameSettingListEntrySetting_Action::HandleActionButtonClicked()
{
	if (ActionSetting)
	{
		// 检查是否需要确认
		if (ActionSetting->GetRequiresConfirmation())
		{
			// 这里可以显示确认对话框
			// 暂时直接执行动作
			ActionSetting->ExecuteAction();
		}
		else
		{
			ActionSetting->ExecuteAction();
		}
	}
}