#include "GameSettingAction.h"
#include "Engine/Engine.h"

UGameSettingAction::UGameSettingAction()
{
	bActionEnabled = true;
}

void UGameSettingAction::ExecuteAction()
{
	if (!CanExecuteAction())
	{
		return;
	}

	// 触发执行前事件
	OnActionExecuting.Broadcast(this);

	// 执行C++委托
	if (ActionDelegate.IsBound())
	{
		ActionDelegate.Execute();
	}

	// 执行蓝图事件
	ReceiveExecuteAction();

	// 触发设置变化事件
	OnSettingChanged.Broadcast(this, EGameSettingChangeReason::Change);

	// 触发执行后事件
	OnActionExecuted.Broadcast(this);
}

bool UGameSettingAction::CanExecuteAction() const
{
	if (!bActionEnabled)
	{
		return false;
	}

	// 检查蓝图实现
	if (GetClass()->IsFunctionImplementedInScript(FName("ReceiveCanExecuteAction")))
	{
		return ReceiveCanExecuteAction();
	}

	return true;
}

void UGameSettingAction::SetActionDelegate(const FGameSettingActionDelegate& InDelegate)
{
	ActionDelegate = InDelegate;
}

void UGameSettingAction::Apply()
{
	// 调用基类的应用逻辑
	OnApply();
	
	// 触发应用事件
	OnSettingApplied.Broadcast(this);
}

void UGameSettingAction::OnApply()
{
	// 动作类型的设置项通常不需要特殊的应用逻辑
	// 但可以在这里添加必要的处理
	
	// 调用基类实现
	Super::OnApply();
}
void UGameSettingAction::SetActionButtonText(const FText& InButtonText)
{
	ActionButtonText = InButtonText;
}

void UGameSettingAction::SetConfirmationText(const FText& InConfirmationText)
{
	ConfirmationText = InConfirmationText;
}

void UGameSettingAction::SetRequiresConfirmation(bool bInRequiresConfirmation)
{
	bRequiresConfirmation = bInRequiresConfirmation;
}