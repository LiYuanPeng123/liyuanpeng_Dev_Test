#include "Widgets/CustomTabContainer.h"
#include "Widgets/CustomTabButton.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Engine.h"

UCustomTabContainer::UCustomTabContainer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 设置默认Tab按钮类
	DefaultTabButtonClass = UCustomTabButton::StaticClass();
}

void UCustomTabContainer::NativeConstruct()
{
	Super::NativeConstruct();

	// 确保容器存在
	if (!HorizontalBox_Tabs)
	{
		UE_LOG(LogTemp, Warning, TEXT("CustomTabContainer: HorizontalBox_Tabs is not bound!"));
	}
}

UCustomTabButton* UCustomTabContainer::RegisterTabButton(const FText& ButtonText, EGameSettingCategory InCategory,TSubclassOf<UCustomTabButton> ButtonClass)
{
	if (!HorizontalBox_Tabs)
	{
		UE_LOG(LogTemp, Error, TEXT("CustomTabContainer: Cannot register tab button - HorizontalBox_Tabs is null"));
		return nullptr;
	}

	if (!ButtonClass)
	{
		UE_LOG(LogTemp, Error, TEXT("CustomTabContainer: Cannot register tab button - ButtonClass is null"));
		return nullptr;
	}

	// 创建新的Tab按钮
	UCustomTabButton* NewTabButton = CreateWidget<UCustomTabButton>(this, ButtonClass);
	if (!NewTabButton)
	{
		UE_LOG(LogTemp, Error, TEXT("CustomTabContainer: Failed to create tab button widget"));
		return nullptr;
	}

	// 设置按钮属性
	NewTabButton->InitializeButton(ButtonText,InCategory,this);
	NewTabButton->OnTabButtonClicked.AddDynamic(this,&ThisClass::OnTabButtonClicked);
	// 添加到容器
	if (UHorizontalBoxSlot* HorizontalBoxSlot = HorizontalBox_Tabs->AddChildToHorizontalBox(NewTabButton))
	{
		HorizontalBoxSlot->SetPadding(FMargin(2.0f, 0.0f));
		HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
		HorizontalBoxSlot->SetVerticalAlignment(VAlign_Fill);
	}

	// 添加到数组
	TabButtons.Add(NewTabButton);

	// 如果是第一个按钮，设置为选中状态
	if (TabButtons.Num() == 1)
	{
		SetSelectedTab(NewTabButton);
	}

	// 触发注册事件
	int32 TabIndex = TabButtons.Num() - 1;
	OnTabButtonRegistered.Broadcast(NewTabButton, TabIndex);

	return NewTabButton;
}

bool UCustomTabContainer::RemoveTabButton(UCustomTabButton* ButtonToRemove)
{
	if (!ButtonToRemove || !HorizontalBox_Tabs)
	{
		return false;
	}

	int32 ButtonIndex = TabButtons.Find(ButtonToRemove);
	if (ButtonIndex == INDEX_NONE)
	{
		return false;
	}

	// 如果移除的是当前选中的按钮，需要选择新的按钮
	if (CurrentSelectedTab == ButtonToRemove)
	{
		if (TabButtons.Num() > 1)
		{
			// 选择下一个按钮，如果是最后一个则选择前一个
			int32 NewSelectedIndex = (ButtonIndex < TabButtons.Num() - 1) ? ButtonIndex : ButtonIndex - 1;
			if (NewSelectedIndex >= 0 && NewSelectedIndex < TabButtons.Num() && NewSelectedIndex != ButtonIndex)
			{
				SetSelectedTab(TabButtons[NewSelectedIndex]);
			}
		}
		else
		{
			CurrentSelectedTab = nullptr;
		}
	}

	// 从UI中移除
	HorizontalBox_Tabs->RemoveChild(ButtonToRemove);

	// 从数组中移除
	TabButtons.RemoveAt(ButtonIndex);

	// 触发移除事件
	OnTabButtonRemoved.Broadcast(ButtonToRemove, ButtonIndex);

	// 销毁按钮
	ButtonToRemove->RemoveFromParent();

	return true;
}

void UCustomTabContainer::ClearAllTabButtons()
{
	if (!HorizontalBox_Tabs)
	{
		return;
	}

	// 清空当前选中
	CurrentSelectedTab = nullptr;

	// 移除所有按钮
	for (int32 i = TabButtons.Num() - 1; i >= 0; --i)
	{
		if (TabButtons[i])
		{
			OnTabButtonRemoved.Broadcast(TabButtons[i], i);
			HorizontalBox_Tabs->RemoveChild(TabButtons[i]);
			TabButtons[i]->RemoveFromParent();
		}
	}

	TabButtons.Empty();
}

void UCustomTabContainer::SetSelectedTab(UCustomTabButton* TabButton)
{
	if (!TabButton || !TabButtons.Contains(TabButton))
	{
		return;
	}

	if (CurrentSelectedTab != TabButton)
	{
		CurrentSelectedTab = TabButton;
		UpdateButtonSelectionStates();

		// 触发选择改变事件
		int32 TabIndex = TabButtons.Find(TabButton);
		OnTabSelectionChanged.Broadcast(TabButton, TabIndex);
	}
}

void UCustomTabContainer::SetSelectedTabByIndex(int32 Index)
{
	if (Index >= 0 && Index < TabButtons.Num())
	{
		SetSelectedTab(TabButtons[Index]);
	}
}

int32 UCustomTabContainer::GetSelectedTabIndex() const
{
	if (CurrentSelectedTab)
	{
		return TabButtons.Find(CurrentSelectedTab);
	}
	return INDEX_NONE;
}

void UCustomTabContainer::NavigateLeft()
{
	if (TabButtons.Num() <= 1)
	{
		return;
	}

	int32 CurrentIndex = GetSelectedTabIndex();
	if (CurrentIndex == INDEX_NONE)
	{
		// 如果没有选中的，选择第一个
		SetSelectedTabByIndex(0);
		return;
	}

	int32 NewIndex;
	if (CurrentIndex > 0)
	{
		NewIndex = CurrentIndex - 1;
	}
	else if (bWrapNavigation)
	{
		NewIndex = TabButtons.Num() - 1;
	}
	else
	{
		return; // 不循环且已经是第一个
	}

	SetSelectedTabByIndex(NewIndex);
}

void UCustomTabContainer::NavigateRight()
{
	if (TabButtons.Num() <= 1)
	{
		return;
	}

	int32 CurrentIndex = GetSelectedTabIndex();
	if (CurrentIndex == INDEX_NONE)
	{
		// 如果没有选中的，选择第一个
		SetSelectedTabByIndex(0);
		return;
	}

	int32 NewIndex;
	if (CurrentIndex < TabButtons.Num() - 1)
	{
		NewIndex = CurrentIndex + 1;
	}
	else if (bWrapNavigation)
	{
		NewIndex = 0;
	}
	else
	{
		return; // 不循环且已经是最后一个
	}

	SetSelectedTabByIndex(NewIndex);
}

UCustomTabButton* UCustomTabContainer::GetTabButtonByIndex(int32 Index) const
{
	if (Index >= 0 && Index < TabButtons.Num())
	{
		return TabButtons[Index];
	}
	return nullptr;
}

void UCustomTabContainer::OnTabButtonClicked(UCustomTabButton* ClickedButton)
{
	SetSelectedTab(ClickedButton);
}

void UCustomTabContainer::UpdateButtonSelectionStates()
{
	for (UCustomTabButton* TabButton : TabButtons)
	{
		if (TabButton)
		{
			TabButton->SetSelected(TabButton == CurrentSelectedTab);
		}
	}
}