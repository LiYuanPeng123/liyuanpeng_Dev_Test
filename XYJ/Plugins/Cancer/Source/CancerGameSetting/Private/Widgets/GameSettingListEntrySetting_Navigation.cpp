#include "Widgets/GameSettingListEntrySetting_Navigation.h"
#include "GameSettingCollection.h"
#include "Components/Button.h"

void UGameSettingListEntrySetting_Navigation::SetSetting(UGameSetting* InSetting)
{
	CollectionSetting = Cast<UGameSettingCollection>(InSetting);
	Super::SetSetting(InSetting);

	if (CollectionSetting)
	{
		// 触发蓝图事件
		OnSettingAssigned(CollectionSetting->GetDisplayName());
	}
}

void UGameSettingListEntrySetting_Navigation::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 绑定按钮事件
	if (Button_Navigate)
	{
		Button_Navigate->OnClicked.AddDynamic(this, &UGameSettingListEntrySetting_Navigation::HandleNavigationButtonClicked);
	}
}

void UGameSettingListEntrySetting_Navigation::NativeOnEntryReleased()
{
	// 清理事件绑定
	if (Button_Navigate)
	{
		Button_Navigate->OnClicked.RemoveAll(this);
	}

	Super::NativeOnEntryReleased();
}

void UGameSettingListEntrySetting_Navigation::HandleNavigationButtonClicked()
{
	if (CollectionSetting)
	{
		// 触发导航事件，让主界面处理页面切换
		OnNavigationRequested(CollectionSetting);
	}
}