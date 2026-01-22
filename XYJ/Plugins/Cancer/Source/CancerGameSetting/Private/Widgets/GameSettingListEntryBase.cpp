// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/GameSettingListEntryBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Widgets/GameSettingListView.h"
#include "Widgets/GameSettingMainWidget.h"

void UGameSettingListEntryBase::SetSetting(UGameSetting* InSetting)
{
	// 如果设置项没有变化，直接返回
	if (Setting == InSetting)
	{
		return;
	}

	// 清理旧的事件绑定
	if (Setting)
	{
		Setting->OnSettingChanged.RemoveAll(this);
	}

	// 设置新的设置项
	Setting = InSetting;

	// 绑定新的事件
	if (Setting)
	{
		Setting->OnSettingChanged.AddUObject(this, &UGameSettingListEntryBase::HandleSettingChanged);
	}
	OnItemSelection(false);
	// 刷新UI显示
	RefreshDisplay();
}

void UGameSettingListEntryBase::SetDisplayNameOverride(const FText& OverrideName)
{
	DisplayNameOverride = OverrideName;
	UpdateDisplayName();
}

bool UGameSettingListEntryBase::IsSettingEditable() const
{
	// 这里可以根据具体需求实现编辑条件检查
	// 例如检查设置项是否被锁定、是否有权限等
	return Setting != nullptr;
}

bool UGameSettingListEntryBase::IsSettingResetable() const
{
	// 检查设置项是否支持重置功能
	if (!Setting)
	{
		return false;
	}

	// 可以添加更多条件，比如检查当前值是否与默认值不同
	return true;
}

void UGameSettingListEntryBase::ResetSettingToDefault()
{
	if (Setting && IsSettingResetable())
	{
		// 如果是值类型设置项，调用重置方法
		if (UGameSettingValue* ValueSetting = Cast<UGameSettingValue>(Setting))
		{
			ValueSetting->ResetToDefault();
		}
	}
}

void UGameSettingListEntryBase::RestoreSettingToInitial()
{
	if (Setting)
	{
		// 如果是值类型设置项，调用恢复方法
		if (UGameSettingValue* ValueSetting = Cast<UGameSettingValue>(Setting))
		{
			ValueSetting->RestoreToInitial();
		}
	}
}

void UGameSettingListEntryBase::RefreshDisplay()
{
	// 更新所有UI元素
	UpdateDisplayName();
	UpdateEnabledState();
	UpdateResetButtonState();

	// 调用设置变更处理
	OnSettingChanged();
}

void UGameSettingListEntryBase::NativeOnEntryReleased()
{
	// 清理设置项事件绑定
	if (Setting)
	{
		Setting->OnSettingChanged.RemoveAll(this);
		Setting = nullptr;
	}
}

void UGameSettingListEntryBase::OnSettingChanged()
{
	// 基类实现为空，子类可以重写此方法来更新特定的UI元素
	// 这里可以添加通用的UI更新逻辑
	UpdateEnabledState();
}

void UGameSettingListEntryBase::HandleEditConditionChanged(UGameSetting* InSetting)
{
	// 处理编辑条件变更
	UpdateEnabledState();
	OnSettingChanged();
}

void UGameSettingListEntryBase::UpdateEnabledState()
{
	bool bIsEditable = IsSettingEditable();

	// 更新整个控件的启用状态
	SetIsEnabled(bIsEditable);

	// 如果设置为禁用时隐藏，则更新可见性
	if (bHideWhenDisabled)
	{
		SetVisibility(bIsEditable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UGameSettingListEntryBase::UpdateDisplayName()
{
	if (!Text_SettingName || !Setting)
	{
		return;
	}

	FText DisplayText;

	// 优先级：自定义名称 > 显示名称 > 开发者名称
	if (!DisplayNameOverride.IsEmpty())
	{
		DisplayText = DisplayNameOverride;
		UE_LOG(LogTemp, VeryVerbose, TEXT("使用自定义显示名称: %s"), *DisplayText.ToString());
	}
	else if (!Setting->GetDisplayName().IsEmpty())
	{
		DisplayText = Setting->GetDisplayName();
		UE_LOG(LogTemp, VeryVerbose, TEXT("使用设置显示名称: %s"), *DisplayText.ToString());
	}
	else
	{
		DisplayText = FText::FromName(Setting->GetDevName());
		UE_LOG(LogTemp, VeryVerbose, TEXT("使用开发者名称: %s"), *DisplayText.ToString());
	}

	Text_SettingName->SetText(DisplayText);
}

void UGameSettingListEntryBase::UpdateResetButtonState()
{
	if (Button_Reset)
	{
		// 根据设置显示/隐藏重置按钮
		bool bShouldShowReset = bShowResetButton && IsSettingResetable();
		Button_Reset->SetVisibility(bShouldShowReset ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

		// 绑定重置按钮事件（如果还没绑定的话）
		if (bShouldShowReset && !Button_Reset->OnClicked.IsBound())
		{
			Button_Reset->OnClicked.AddDynamic(this, &UGameSettingListEntryBase::OnResetButtonClicked);
		}
	}
}

void UGameSettingListEntryBase::OnResetButtonClicked()
{
	// 重置设置项到默认值
	ResetSettingToDefault();
}

void UGameSettingListEntryBase::HandleSettingChanged(UGameSetting* InSetting, EGameSettingChangeReason Reason)
{
	// 处理设置项变更事件
	OnSettingChanged();

	// 向上传递事件到父级列表视图
	if (UGameSettingListView* ListView = Cast<UGameSettingListView>(GetOwningListView()))
	{
		if (UGameSettingMainWidget* MainWidget = ListView->GetTypedOuter<UGameSettingMainWidget>())
		{
			MainWidget->OnSettingChanged(InSetting, Reason);
		}
	}

	// 根据变更原因执行不同的逻辑
	switch (Reason)
	{
	case EGameSettingChangeReason::Change:
		// 用户主动变更
		break;
	case EGameSettingChangeReason::DependencyChanged:
		// 依赖项变更，可能需要更新UI状态
		UpdateEnabledState();
		break;
	case EGameSettingChangeReason::ResetToDefault:
	case EGameSettingChangeReason::RestoreToInitial:
		// 重置或恢复操作，刷新整个显示
		RefreshDisplay();
		break;
	}
}

void UGameSettingListEntryBase::NativeOnItemSelectionChanged(bool bIsSelected)
{
	OnItemSelection(bIsSelected);
}

void UGameSettingListEntryBase::OnItemSelection(bool IsSelection)
{
	// 如果没有选中框Image控件，直接返回
	if (!Image_SelectionIndicator)
	{
		return;
	}
	UMaterialInterface* TargetMaterial = nullptr;
	bCurIsSelected = IsSelection;
	if (IsSelection)
	{
		TargetMaterial = SelectedMaterial;
	}
	else
	{
		TargetMaterial = UnselectedMaterial;
	}

	// 应用材质
	if (TargetMaterial)
	{
		Image_SelectionIndicator->SetBrushFromMaterial(TargetMaterial);
		Image_SelectionIndicator->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		// 如果没有合适的材质，隐藏选中框
		Image_SelectionIndicator->SetVisibility(ESlateVisibility::Collapsed);
	}
	HandleItemSelection(IsSelection);
}

void UGameSettingListEntryBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	// 获取父级ListView
	if (UGameSettingListView* ParentListView = Cast<UGameSettingListView>(GetOwningListView()))
	{
		if (ParentListView->GetEnableHoverSelection())
		{
			// 设置当前项为选中项
			UObject* MyData = GetListItem();
			if (MyData)
			{
				ParentListView->SetSelectedItem(MyData);
				// 更新选中索引
				int32 ItemIndex = ParentListView->GetIndexForItem(MyData);
				if (ItemIndex != INDEX_NONE)
				{
					ParentListView->SelectItemByIndex(ItemIndex);
				}
			}
		}
	}
}

void UGameSettingListEntryBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	// 可以在这里添加鼠标离开的处理逻辑
}


// UGameSettingListEntry_Setting 实现

void UGameSettingListEntry_Setting::SetSetting(UGameSetting* InSetting)
{
	// 调用基类方法
	Super::SetSetting(InSetting);

	// 更新设置项名称显示
	UpdateDisplayName();
}

void UGameSettingListEntry_Setting::OnSettingChanged()
{
	// 调用基类方法
	Super::OnSettingChanged();

	// 更新设置项名称（可能因为DisplayNameOverride变化）
	UpdateDisplayName();
}

