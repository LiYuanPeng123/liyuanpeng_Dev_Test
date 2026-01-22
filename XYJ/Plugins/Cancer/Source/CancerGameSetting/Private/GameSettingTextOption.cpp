
#include "GameSettingTextOption.h"

UGameSettingTextOption::UGameSettingTextOption()
{
	// 初始化默认值
	CurrentIndex = 0;
	DefaultIndex = 0;
	InitialIndex = 0;
}

void UGameSettingTextOption::SetOptions(const TArray<FText>& InOptions)
{
	Options = InOptions;
	
	// 确保当前索引在有效范围内
	if (!IsValidIndex(CurrentIndex))
	{
		CurrentIndex = 0;
	}
	
	// 确保默认索引在有效范围内
	if (!IsValidIndex(DefaultIndex))
	{
		DefaultIndex = 0;
	}
}

void UGameSettingTextOption::AddOption(const FText& Option)
{
	Options.Add(Option);
}

void UGameSettingTextOption::ClearOptions()
{
	Options.Empty();
	CurrentIndex = 0;
	DefaultIndex = 0;
	InitialIndex = 0;
}

void UGameSettingTextOption::SetDiscreteOptionByIndex(int32 Index)
{
	if (IsValidIndex(Index) && Index != CurrentIndex)
	{
		CurrentIndex = Index;
		NotifySettingChanged(EGameSettingChangeReason::Change);
	}
}

int32 UGameSettingTextOption::GetDiscreteOptionIndex() const
{
	return CurrentIndex;
}

int32 UGameSettingTextOption::GetDiscreteOptionDefaultIndex() const
{
	return DefaultIndex;
}

TArray<FText> UGameSettingTextOption::GetDiscreteOptions() const
{
	return Options;
}

FText UGameSettingTextOption::GetSelectedText() const
{
	if (IsValidIndex(CurrentIndex))
	{
		return Options[CurrentIndex];
	}
	return FText::GetEmpty();
}

bool UGameSettingTextOption::SetSelectedByText(const FText& Text)
{
	for (int32 i = 0; i < Options.Num(); ++i)
	{
		if (Options[i].EqualTo(Text))
		{
			SetDiscreteOptionByIndex(i);
			return true;
		}
	}
	return false;
}

void UGameSettingTextOption::SetDefaultIndex(int32 Index)
{
	if (IsValidIndex(Index))
	{
		DefaultIndex = Index;
	}
}

void UGameSettingTextOption::OnInitialized()
{
	Super::OnInitialized();
	
	// 存储初始值
	StoreInitial();
}

void UGameSettingTextOption::StoreInitial()
{
	Super::StoreInitial();
	InitialIndex = CurrentIndex;
}

void UGameSettingTextOption::ResetToDefault()
{
	Super::ResetToDefault();
	
	if (CurrentIndex != DefaultIndex)
	{
		CurrentIndex = DefaultIndex;
		NotifySettingChanged(EGameSettingChangeReason::ResetToDefault);
	}
}

void UGameSettingTextOption::RestoreToInitial()
{
	Super::RestoreToInitial();
	
	if (CurrentIndex != InitialIndex)
	{
		CurrentIndex = InitialIndex;
		NotifySettingChanged(EGameSettingChangeReason::RestoreToInitial);
	}
}

bool UGameSettingTextOption::IsValidIndex(int32 Index) const
{
	return Index >= 0 && Index < Options.Num();
}

void UGameSettingTextOption::NotifySettingChanged(EGameSettingChangeReason Reason)
{
	// 触发设置变更事件
	OnSettingChanged.Broadcast(this, Reason);
}