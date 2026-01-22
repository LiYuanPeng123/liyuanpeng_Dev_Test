
#include "GameSettingCollection.h"

UGameSettingCollection::UGameSettingCollection()
{
	// 构造函数实现
}

TArray<UGameSettingCollection*> UGameSettingCollection::GetChildCollections() const
{
	TArray<UGameSettingCollection*> Collections;
	for (UGameSetting* Setting : Settings)
	{
		if (UGameSettingCollection* Collection = Cast<UGameSettingCollection>(Setting))
		{
			Collections.Add(Collection);
		}
	}
	return Collections;
}

void UGameSettingCollection::AddSetting(UGameSetting* Setting)
{
	if (Setting && !Settings.Contains(Setting))
	{
		Settings.Add(Setting);
		
		// 设置父级关系
		Setting->SetParentSetting(this);
		
		// 触发设置变化事件
		OnSettingChanged.Broadcast(this, EGameSettingChangeReason::Change);
	}
}

void UGameSettingCollection::RemoveSetting(UGameSetting* Setting)
{
	if (Setting && Settings.Contains(Setting))
	{
		Settings.Remove(Setting);
		
		// 清除父级关系
		Setting->SetParentSetting(nullptr);
		
		// 触发设置变化事件
		OnSettingChanged.Broadcast(this, EGameSettingChangeReason::Change);
	}
}

TArray<UGameSetting*> UGameSettingCollection::GetSettingsForFilter(const FGameSettingFilterState& FilterState) const
{
	TArray<UGameSetting*> FilteredSettings;
	
	for (UGameSetting* Setting : Settings)
	{
		if (Setting && FilterState.DoesSettingPassFilter(*Setting))
		{
			FilteredSettings.Add(Setting);
		}
	}
	
	return FilteredSettings;
}

void UGameSettingCollection::ClearSettings()
{
	// 清除所有设置项的父级关系
	for (UGameSetting* Setting : Settings)
	{
		if (Setting)
		{
			Setting->SetParentSetting(nullptr);
		}
	}
	
	Settings.Empty();
	
	// 触发设置变化事件
	OnSettingChanged.Broadcast(this, EGameSettingChangeReason::Change);
}

UGameSetting* UGameSettingCollection::FindSettingByDevName(const FName& InDevName) const
{
	for (UGameSetting* Setting : Settings)
	{
		if (Setting && Setting->GetDevName() == InDevName)
		{
			return Setting;
		}
	}
	
	return nullptr;
}
