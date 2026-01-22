#include "Widgets/CustomTabButton.h"
#include "Widgets/CustomTabContainer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Materials/MaterialInterface.h"

UCustomTabButton::UCustomTabButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsSelected = false;
	bIsHovered = false;
	bIsPressed = false;
	bIsEnabled = true;
	Category = EGameSettingCategory::Graphics;
	ParentContainer = nullptr;
}

void UCustomTabButton::NativeConstruct()
{
	Super::NativeConstruct();

	// 绑定按钮事件
	if (MainButton)
	{
		MainButton->OnClicked.AddDynamic(this, &UCustomTabButton::OnButtonClicked);
		MainButton->OnHovered.AddDynamic(this, &UCustomTabButton::OnButtonHovered);
		MainButton->OnUnhovered.AddDynamic(this, &UCustomTabButton::OnButtonUnhovered);
		MainButton->OnPressed.AddDynamic(this, &UCustomTabButton::OnButtonPressed);
		MainButton->OnReleased.AddDynamic(this, &UCustomTabButton::OnButtonReleased);
	}

	// 初始化视觉状态
	UpdateVisualState();
}

void UCustomTabButton::NativeDestruct()
{
	// 清理事件绑定
	if (MainButton)
	{
		MainButton->OnClicked.RemoveAll(this);
		MainButton->OnHovered.RemoveAll(this);
		MainButton->OnUnhovered.RemoveAll(this);
		MainButton->OnPressed.RemoveAll(this);
		MainButton->OnReleased.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UCustomTabButton::InitializeButton(const FText& InText, EGameSettingCategory InCategory,
	UCustomTabContainer* InContainer)
{
	SetButtonText(InText);
	Category = InCategory;
	ParentContainer = InContainer;
	// 更新视觉状态
	UpdateVisualState();
}

void UCustomTabButton::SetButtonText(const FText& InText)
{
	if (ButtonText)
	{
		ButtonText->SetText(InText);
	}
}

FText UCustomTabButton::GetButtonText() const
{
	if (ButtonText)
	{
		return ButtonText->GetText();
	}
	return FText::GetEmpty();
}

void UCustomTabButton::SetSelected(bool bInSelected)
{
	if (bIsSelected != bInSelected)
	{
		bIsSelected = bInSelected;
		UpdateVisualState();
	}
}

void UCustomTabButton::SetButtonEnabled(bool bInEnabled)
{
	if (bIsEnabled != bInEnabled)
	{
		bIsEnabled = bInEnabled;
		if (MainButton)
		{
			MainButton->SetIsEnabled(bInEnabled);
		}
		UpdateVisualState();
	}
}

void UCustomTabButton::SetTextColor(FSlateColor InColor)
{
	NormalTextColor = InColor;
	UpdateVisualState();
}

void UCustomTabButton::SetHoveredTextColor(FSlateColor InColor)
{
	HoveredTextColor = InColor;
	UpdateVisualState();
}

void UCustomTabButton::SetSelectedTextColor(FSlateColor InColor)
{
	SelectedTextColor = InColor;
	UpdateVisualState();
}

void UCustomTabButton::SetDisabledTextColor(FSlateColor InColor)
{
	DisabledTextColor = InColor;
	UpdateVisualState();
}

void UCustomTabButton::ApplyTextColor(const FSlateColor& InColor)
{
	if (ButtonText)
	{
		ButtonText->SetColorAndOpacity(InColor);
	}
}

void UCustomTabButton::OnButtonClicked()
{
	if (!bIsEnabled)
	{
		return;
	}

	// 广播点击事件，传递按钮自身和关联的分类
	OnTabButtonClicked.Broadcast(this);
}

void UCustomTabButton::OnButtonHovered()
{
	bIsHovered = true;
	UpdateVisualState();
}

void UCustomTabButton::OnButtonUnhovered()
{
	bIsHovered = false;
	UpdateVisualState();
}

void UCustomTabButton::OnButtonPressed()
{
	bIsPressed = true;
	UpdateVisualState();
}

void UCustomTabButton::OnButtonReleased()
{
	bIsPressed = false;
	UpdateVisualState();
}

void UCustomTabButton::UpdateVisualState()
{
	if (!BackgroundImage)
	{
		return;
	}

	UMaterialInterface* MaterialToUse = nullptr;
	FSlateColor TextColorToUse = NormalTextColor;

	// 按优先级确定要使用的材质和文本颜色
	if (!bIsEnabled && DisabledMaterial)
	{
		MaterialToUse = DisabledMaterial;
		TextColorToUse = DisabledTextColor;
	}
	else if (bIsSelected && SelectedMaterial)
	{
		MaterialToUse = SelectedMaterial;
		TextColorToUse = SelectedTextColor;
	}
	else if (bIsPressed && PressedMaterial)
	{
		MaterialToUse = PressedMaterial;
		TextColorToUse = PressedTextColor;
	}
	else if (bIsHovered && HoveredMaterial)
	{
		MaterialToUse = HoveredMaterial;
		TextColorToUse = HoveredTextColor;
	}
	else if (NormalMaterial)
	{
		MaterialToUse = NormalMaterial;
		TextColorToUse = NormalTextColor;
	}

	// 应用材质
	if (MaterialToUse)
	{
		BackgroundImage->SetBrushFromMaterial(MaterialToUse);
	}

	// 应用文本颜色
	ApplyTextColor(TextColorToUse);
}

// 材质设置方法实现
void UCustomTabButton::SetNormalMaterial(UMaterialInterface* InMaterial)
{
	NormalMaterial = InMaterial;
	UpdateVisualState();
}

void UCustomTabButton::SetHoveredMaterial(UMaterialInterface* InMaterial)
{
	HoveredMaterial = InMaterial;
	UpdateVisualState();
}

void UCustomTabButton::SetPressedMaterial(UMaterialInterface* InMaterial)
{
	PressedMaterial = InMaterial;
	UpdateVisualState();
}

void UCustomTabButton::SetSelectedMaterial(UMaterialInterface* InMaterial)
{
	SelectedMaterial = InMaterial;
	UpdateVisualState();
}

void UCustomTabButton::SetDisabledMaterial(UMaterialInterface* InMaterial)
{
	DisabledMaterial = InMaterial;
	UpdateVisualState();
}