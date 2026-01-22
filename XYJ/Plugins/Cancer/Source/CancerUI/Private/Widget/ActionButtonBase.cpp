#include "Widget/ActionButtonBase.h"
#include "Widget/CancerButtonBase.h"

UActionButtonBase::UActionButtonBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 设置默认值
}

void UActionButtonBase::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 绑定按钮点击事件
	if (ActionButton)
	{
		ActionButton->OnClicked.AddDynamic(this, &UActionButtonBase::OnActionButtonClicked);
		// 绑定双击事件
		ActionButton->OnDoubleClicked.AddDynamic(this, &UActionButtonBase::OnActionButtonDoubleClicked);
	}
}

void UActionButtonBase::OnActionButtonClicked()
{
	// 调用蓝图可重写的事件
	OnButtonClicked();
}

void UActionButtonBase::SetButtonEnabled(bool bEnabled)
{
	if (ActionButton)
	{
		ActionButton->SetButtonEnabled(bEnabled);
	}
}

void UActionButtonBase::SetInteractionEnabled(bool bEnabled)
{
	if (ActionButton)
	{
		ActionButton->SetInteractionEnabled(bEnabled);
	}
}

void UActionButtonBase::SetButtonFocusable(bool bFocusable)
{
	if (ActionButton)
	{
		ActionButton->SetButtonFocusable(bFocusable);
	}
}

bool UActionButtonBase::IsButtonHovered() const
{
	if (ActionButton)
	{
		return ActionButton->IsHovered();
	}
	return false;
}

bool UActionButtonBase::IsButtonPressed() const
{
	if (ActionButton)
	{
		return ActionButton->IsPressed();
	}
	return false;
}

void UActionButtonBase::OnActionButtonDoubleClicked()
{
	// 调用蓝图可重写的双击事件
	OnButtonDoubleClicked();
}
