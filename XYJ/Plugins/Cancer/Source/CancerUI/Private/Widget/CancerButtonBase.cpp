#include "Widget/CancerButtonBase.h"

#include "Components/ButtonSlot.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"

UCancerButtonBase::UCancerButtonBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bButtonEnabled(true)
	, bInteractionEnabled(true)
	, bButtonFocusable(true)
{
	// 设置默认值
	MinWidth = 50;
	MinHeight = 50;
	MaxWidth = 100;
	MaxHeight = 100;
	LastClickTime = 0.0;
}

void UCancerButtonBase::SetButtonEnabled(bool bInIsButtonEnabled)
{
	bButtonEnabled = bInIsButtonEnabled;
	if (MyCancerButton.IsValid())
	{
		MyCancerButton->SetEnabled(bInIsButtonEnabled);
	}
}

void UCancerButtonBase::SetInteractionEnabled(bool bInIsInteractionEnabled)
{
	if (bInteractionEnabled == bInIsInteractionEnabled)
	{
		return;
	}

	bInteractionEnabled = bInIsInteractionEnabled;
	// 注意：SButton 没有直接的 SetIsInteractionEnabled 方法
	// 我们通过启用/禁用来模拟这个功能
	if (MyCancerButton.IsValid())
	{
		MyCancerButton->SetEnabled(bButtonEnabled && bInteractionEnabled);
	}
}

void UCancerButtonBase::SetButtonFocusable(bool bInIsButtonFocusable)
{
	bButtonFocusable = bInIsButtonFocusable;
	if (MyCancerButton.IsValid())
	{
		MyCancerButton->SetCanTick(bButtonFocusable);
	}
}

bool UCancerButtonBase::IsHovered() const
{
	if (MyCancerButton.IsValid())
	{
		return MyCancerButton->IsHovered();
	}
	return false;
}

bool UCancerButtonBase::IsPressed() const
{
	if (MyCancerButton.IsValid())
	{
		return MyCancerButton->IsPressed();
	}
	return false;
}

void UCancerButtonBase::SetMinDesiredHeight(int32 InMinHeight)
{
	MinHeight = InMinHeight;
	if (MyBox.IsValid())
	{
		MyBox->SetMinDesiredHeight(InMinHeight);
	}
}

void UCancerButtonBase::SetMinDesiredWidth(int32 InMinWidth)
{
	MinWidth = InMinWidth;
	if (MyBox.IsValid())
	{
		MyBox->SetMinDesiredWidth(InMinWidth);
	}
}

void UCancerButtonBase::SetMaxDesiredHeight(int32 InMaxHeight)
{
	MaxHeight = InMaxHeight;
	if (MyBox.IsValid())
	{
		MyBox->SetMaxDesiredHeight(InMaxHeight > 0 ? InMaxHeight : FOptionalSize());
	}
}

void UCancerButtonBase::SetMaxDesiredWidth(int32 InMaxWidth)
{
	MaxWidth = InMaxWidth;
	if (MyBox.IsValid())
	{
		MyBox->SetMaxDesiredWidth(InMaxWidth > 0 ? InMaxWidth : FOptionalSize());
	}
}

TSharedRef<SWidget> UCancerButtonBase::RebuildWidget()
{
	// 创建 SButton
	MyButton = MyCancerButton = SNew(SButton)
		.OnClicked_UObject(this, &UCancerButtonBase::SlateHandleClicked)
		.OnPressed_UObject(this, &UCancerButtonBase::SlateHandlePressed)
		.OnReleased_UObject(this, &UCancerButtonBase::SlateHandleReleased)
		.ButtonStyle(&GetStyle())
		.ClickMethod(GetClickMethod())
		.TouchMethod(GetTouchMethod())
		.IsFocusable(bButtonFocusable)
		.IsEnabled(bButtonEnabled && bInteractionEnabled);

	// 创建 SBox 容器
	MyBox = SNew(SBox)
		.MinDesiredWidth(MinWidth)
		.MinDesiredHeight(MinHeight)
		.MaxDesiredWidth(MaxWidth > 0 ? MaxWidth : FOptionalSize())
		.MaxDesiredHeight(MaxHeight > 0 ? MaxHeight : FOptionalSize())
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			MyCancerButton.ToSharedRef()
		];

	// 如果有子内容，添加到按钮中
	if (GetChildrenCount() > 0)
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyCancerButton.ToSharedRef());
	}

	return MyBox.ToSharedRef();
}

void UCancerButtonBase::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyCancerButton.Reset();
	MyBox.Reset();
}

FReply UCancerButtonBase::SlateHandleClicked()
{
	// 检查是否允许交互
	if (!bInteractionEnabled || !bButtonEnabled)
	{
		return FReply::Handled();
	}

	// 双击检测
	double CurrentTime = FPlatformTime::Seconds();
	if (CurrentTime - LastClickTime <= DoubleClickTimeThreshold)
	{
		// 触发双击事件
		SlateHandleDoubleClicked();
		LastClickTime = 0.0; // 重置时间避免三击
		return FReply::Handled();
	}
	else
	{
		LastClickTime = CurrentTime;
		// 调用父类的点击处理
		return Super::SlateHandleClicked();
	}
}

FReply UCancerButtonBase::SlateHandleDoubleClicked()
{
	// 触发蓝图双击事件
	OnDoubleClicked.Broadcast();
	return FReply::Handled();
}

void UCancerButtonBase::SlateHandlePressed()
{
	Super::SlateHandlePressed();
}

void UCancerButtonBase::SlateHandleReleased()
{
	Super::SlateHandleReleased();
}

void UCancerButtonBase::HandleDoubleClicked()
{
	// 触发蓝图双击事件
	OnDoubleClicked.Broadcast();
}

void UCancerButtonBase::TriggerDoubleClick()
{
	// 手动触发双击事件（用于测试或蓝图调用）
	OnDoubleClicked.Broadcast();
}
