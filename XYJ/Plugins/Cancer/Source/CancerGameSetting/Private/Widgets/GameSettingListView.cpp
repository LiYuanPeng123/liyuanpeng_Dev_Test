
#include "Widgets/GameSettingListView.h"

#include "GameSettingCollection.h"
#include "GameSettingValueScalar.h"
#include "Editor/WidgetCompilerLog.h"
#include "Widgets/GameSettingListEntryBase.h"
#include "Widgets/GameSettingListEntrySetting_Discrete.h"
#include "Widgets/GameSettingListEntrySetting_Scalar.h"
#include "Widgets/GameSettingVisualData.h"

UGameSettingListView::UGameSettingListView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGameSettingListView::AddNameOverride(const FName& DevName, const FText& OverrideName)
{
	NameOverrides.Add(DevName, OverrideName);
}


#if WITH_EDITOR

void UGameSettingListView::ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const
{
	Super::ValidateCompiledDefaults(InCompileLog);

	if (!VisualData)
	{
		InCompileLog.Error(FText::Format(FText::FromString("{0} has no VisualData defined."), FText::FromString(GetName())));
	}
}

#endif


UUserWidget& UGameSettingListView::OnGenerateEntryWidgetInternal(UObject* Item,
	TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	UGameSetting* SettingItem = Cast<UGameSetting>(Item);

	TSubclassOf<UGameSettingListEntryBase> SettingEntryClass = TSubclassOf<UGameSettingListEntryBase>(DesiredEntryClass);
	if (VisualData)
	{
		if (const TSubclassOf<UGameSettingListEntryBase> EntryClassSetting = VisualData->GetEntryForSetting(SettingItem))
		{
			SettingEntryClass = EntryClassSetting;
		}
		else
		{
			//UE_LOG(LogGameSettings, Error, TEXT("UGameSettingListView: No Entry Class Found!"));
		}
	}
	else
	{
		//UE_LOG(LogGameSettings, Error, TEXT("UGameSettingListView: No VisualData Defined!"));
	}

	UGameSettingListEntryBase& EntryWidget = GenerateTypedEntry<UGameSettingListEntryBase>(SettingEntryClass, OwnerTable);

	if (!IsDesignTime())
	{
		if (const FText* Override = NameOverrides.Find(SettingItem->GetDevName()))
		{
			EntryWidget.SetDisplayNameOverride(*Override);
		}

		EntryWidget.SetSetting(SettingItem);
	}

	return EntryWidget;
}

bool UGameSettingListView::OnIsSelectableOrNavigableInternal(UObject* SelectedItem)
{
	if (const UGameSettingCollection* CollectionItem = Cast<UGameSettingCollection>(SelectedItem))
	{
		return CollectionItem->IsSelectable();
	}

	return true;
}

void UGameSettingListView::SetSettingCollection(UGameSettingCollection* InCollection)
{
	if (!InCollection)
	{
		ClearListItems();
		return;
	}

	// 清空现有项目
	ClearListItems();

	// 添加集合中的所有设置项
	TArray<UGameSetting*> Settings = InCollection->GetChildSettings();
	for (UGameSetting* Setting : Settings)
	{
		if (Setting)
		{
			AddItem(Setting);
		}
	}

	// 刷新列表
	RegenerateAllEntries();
	
	// 自动选择第一项
	if (bAutoSelectFirstItem && GetNumItems() > 0)
	{
		SelectFirstItem();
	}
}

// 新增：导航功能实现
void UGameSettingListView::NavigateUp()
{
	int32 NumItems = GetNumItems();
	if (NumItems == 0) return;
	
	int32 NewIndex = CurrentSelectedIndex - 1;
	if (NewIndex < 0)
	{
		NewIndex = NumItems - 1; // 循环到最后一项
	}
	
	SelectItemByIndex(NewIndex);
}

void UGameSettingListView::NavigateDown()
{
	int32 NumItems = GetNumItems();
	if (NumItems == 0) return;
	
	int32 NewIndex = CurrentSelectedIndex + 1;
	if (NewIndex >= NumItems)
	{
		NewIndex = 0; // 循环到第一项
	}
	
	SelectItemByIndex(NewIndex);
}

void UGameSettingListView::SelectFirstItem()
{
	if (GetNumItems() > 0)
	{
		SelectItemByIndex(0);
	}
}

void UGameSettingListView::SelectItemByIndex(int32 Index)
{
	int32 NumItems = GetNumItems();
	if (Index < 0 || Index >= NumItems) return;
	
	CurrentSelectedIndex = Index;
	
	// 获取对应的数据项
	SetSelectedIndex(Index);
}

int32 UGameSettingListView::GetSelectedIndex() const
{
	return CurrentSelectedIndex;
}

UGameSetting* UGameSettingListView::GetCurrentSelectedSetting() const
{
	UObject* SelectedItem = GetSelectedItem();
	return Cast<UGameSetting>(SelectedItem);
}

UGameSettingListEntryBase* UGameSettingListView::GetCurrentSelectedEntryWidget() const
{
	UObject* SelectedItem = GetSelectedItem();
	if (SelectedItem)
	{
		UUserWidget* EntryWidget = GetEntryWidgetFromItem(SelectedItem);
		return Cast<UGameSettingListEntryBase>(EntryWidget);
	}
	return nullptr;
}

bool UGameSettingListView::AdjustCurrentScalarSetting(float DeltaValue)
{
	UGameSettingListEntryBase* CurrentEntry = GetCurrentSelectedEntryWidget();
	if (!CurrentEntry)
	{
		return false;
	}
	
	// 检查是否为Scalar类型的Entry
	UGameSettingListEntrySetting_Scalar* ScalarEntry = Cast<UGameSettingListEntrySetting_Scalar>(CurrentEntry);
	if (!ScalarEntry || !ScalarEntry->ScalarSetting)
	{
		return false;
	}
	
	// 获取当前值并应用增量
	float CurrentValue = ScalarEntry->ScalarSetting->GetValue();
	float NewValue = CurrentValue + DeltaValue;
	
	// 限制在有效范围内
	float MinValue = ScalarEntry->ScalarSetting->GetMinimumValue();
	float MaxValue = ScalarEntry->ScalarSetting->GetMaximumValue();
	NewValue = FMath::Clamp(NewValue, MinValue, MaxValue);
	
	// 设置新值
	ScalarEntry->ScalarSetting->SetValue(NewValue);
	
	// 刷新显示
	ScalarEntry->Refresh();
	
	return true;
}

bool UGameSettingListView::AdjustCurrentDiscreteSetting(bool bIncrease)
{
	UGameSettingListEntryBase* CurrentEntry = GetCurrentSelectedEntryWidget();
	if (!CurrentEntry)
	{
		return false;
	}
	
	// 检查是否为Discrete类型的Entry
	UGameSettingListEntrySetting_Discrete* DiscreteEntry = Cast<UGameSettingListEntrySetting_Discrete>(CurrentEntry);
	if (!DiscreteEntry || !DiscreteEntry->DiscreteSetting)
	{
		return false;
	}
	
	// 手动调用增加或减少方法
	if (bIncrease)
	{
		DiscreteEntry->HandleOptionIncrease();
	}
	else
	{
		DiscreteEntry->HandleOptionDecrease();
	}
	
	return true;
}
