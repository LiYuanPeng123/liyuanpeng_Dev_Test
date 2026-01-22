#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SEditableTextBox;

/**
 * 资产前缀/后缀编辑窗口
 * 用于批量编辑资产的前缀和后缀
 */
class CANCERASSETEDITOR_API SAssetPrefixSuffixWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetPrefixSuffixWindow)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	/** UI控件 */
	TSharedPtr<SEditableTextBox> PrefixInputBox;
	TSharedPtr<SEditableTextBox> SuffixInputBox;

	/** 按钮点击事件处理函数 */
	FReply OnAddClicked();
	FReply OnRemoveClicked();
	FReply OnCancelClicked();

	/** 获取输入的前缀和后缀 */
	FString GetPrefixText() const;
	FString GetSuffixText() const;
    
	/** 获取复选框状态 */
	bool GetSkipExistingState() const;
	bool GetExactMatchState() const;
}; 