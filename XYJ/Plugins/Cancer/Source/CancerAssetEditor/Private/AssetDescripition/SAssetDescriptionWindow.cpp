#include "AssetDescripition/SAssetDescriptionWindow.h"

#include "CancerAssetFunctionLibrary.h"
#include "AssetDescripition/AssetDescription.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"


/**
 * 构造函数：创建并初始化窗口的UI界面
 * @param InArgs - 包含窗口创建参数的结构体
 */

static FName KeyName = " Description";
void SAssetDescriptionWindow::Construct(const FArguments& InArgs)
{
	  // 初始化成员变量
    bEnableSequentialEdit = InArgs._bEnableSequentialEdit;  // 设置编辑模式（批量/序列）
    CurrentAssetIndex = 0;                                  // 初始化当前资产索引

    // 构建窗口UI布局
    ChildSlot
    [
        SNew(SVerticalBox)  // 创建垂直布局容器
        // 1. 资产信息显示区域
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5.0f)
        [
            SNew(STextBlock)
            .Text(this, &SAssetDescriptionWindow::GetCurrentAssetText)  // 绑定动态文本获取函数
        ]
        // 2. 当前资产的BlueprintDescription显示区域
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.0f, 0.0f, 5.0f, 0.0f)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("当前描述: ")))
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(STextBlock)
                .Text(this, &SAssetDescriptionWindow::GetCurrentBlueprintDescription)
                .IsEnabled_Lambda([this]()
                {
                    return !UCancerAssetFunctionLibrary::IsFirstCharDigit(GetCurrentDescription());
                })
                .AutoWrapText(true)
            ]
        ]
        // 3. 描述编辑区域
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .Padding(5.0f)
        [
            SNew(SScrollBox)  // 创建可滚动容器
            + SScrollBox::Slot()
            [
                SAssignNew(DescriptionTextBox, SMultiLineEditableTextBox)  // 创建多行文本编辑框
                .Text(FText::FromString(TEXT("")))
                .AutoWrapText(true)  // 启用自动换行
            ]
        ]
        // 4. 按钮区域
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5.0f)
        [
            SNew(SHorizontalBox)  // 创建水平布局容器
            // 导航按钮（仅在序列模式下显示）
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.0f, 0.0f, 5.0f, 0.0f)
            [
                SNew(SButton)
                .Text(FText::FromString(TEXT("上一个")))
                .OnClicked(this, &SAssetDescriptionWindow::OnPreviousAssetClicked)
                .IsEnabled(this, &SAssetDescriptionWindow::CanNavigatePrevious)
                .Visibility(bEnableSequentialEdit ? EVisibility::Visible : EVisibility::Collapsed)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.0f, 0.0f, 5.0f, 0.0f)
            [
                SNew(SButton)
                .Text(FText::FromString(TEXT("下一个")))
                .OnClicked(this, &SAssetDescriptionWindow::OnNextAssetClicked)
                .IsEnabled(this, &SAssetDescriptionWindow::CanNavigateNext)
                .Visibility(bEnableSequentialEdit ? EVisibility::Visible : EVisibility::Collapsed)
            ]
            // 填充空间
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            // 确认/取消按钮
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5.0f, 0.0f)
            [
                SNew(SButton)
                .Text(FText::FromString(TEXT("确认")))
                .OnClicked(this, &SAssetDescriptionWindow::OnConfirmClicked)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .Text(FText::FromString(TEXT("取消")))
                .OnClicked(this, &SAssetDescriptionWindow::OnCancelClicked)
            ]
        ]
    ];
}

/**
 * 确认按钮点击处理函数
 * 根据编辑模式执行不同的保存逻辑
 */

FReply SAssetDescriptionWindow::OnConfirmClicked()
{
    if (bEnableSequentialEdit)
    {
        // 序列模式：只保存当前正在编辑的资产
        SaveCurrentAssetDescription();
    }
    else
    {
        // 批量模式：将当前描述应用到所有选中的资产
        FString NewDescription = GetCurrentDescription();
        for (FAssetData& AssetData : SelectedAssets)
        {
            if (AssetData.IsValid())
            {
                
                UAssetDescriptionManager::GetInstance()->SetAssetDescription
                 (AssetData,NewDescription);
                
            }
        }
    }
    
    // 关闭窗口
    if (TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared()))
    {
        ParentWindow->RequestDestroyWindow();
    }
    
    return FReply::Handled();
}

/**
 * 取消按钮点击处理函数
 * 直接关闭窗口，不保存任何更改
 */

FReply SAssetDescriptionWindow::OnCancelClicked()
{
    if (TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared()))
    {
        ParentWindow->RequestDestroyWindow();
    }
    return FReply::Handled();
}

/**
 * 下一个资产按钮点击处理函数
 * 保存当前资产并切换到下一个资产
 */
FReply SAssetDescriptionWindow::OnNextAssetClicked()
{
    // 保存当前资产并确保还有下一个资产可以切换
    if (SaveCurrentAssetDescription() && CurrentAssetIndex < SelectedAssets.Num() - 1)
    {
        CurrentAssetIndex++;
        LoadAssetDescription(CurrentAssetIndex);  // 加载下一个资产的描述
        UpdateWindowTitle();                      // 更新窗口标题显示当前资产信息
    }
    return FReply::Handled();
}

/**
 * 上一个资产按钮点击处理函数
 * 保存当前资产并切换到上一个资产
 */
FReply SAssetDescriptionWindow::OnPreviousAssetClicked()
{
    // 保存当前资产并确保还有上一个资产可以切换
    if (SaveCurrentAssetDescription() && CurrentAssetIndex > 0)
    {
        CurrentAssetIndex--;
        LoadAssetDescription(CurrentAssetIndex);  // 加载上一个资产的描述
        UpdateWindowTitle();                      // 更新窗口标题显示当前资产信息
    }
    return FReply::Handled();
}

FString SAssetDescriptionWindow::GetCurrentDescription() const
{
    return DescriptionTextBox->GetText().ToString();
}

/**
 * 设置要编辑的资产列表
 * @param InAssets - 要编辑的资产数组
 */
void SAssetDescriptionWindow::SetAssetDescriptions(const TArray<FAssetData>& InAssets)
{
    SelectedAssets = InAssets;
    CurrentAssetIndex = 0;
    if (InAssets.Num() > 0)
    {
        LoadAssetDescription(CurrentAssetIndex);  // 加载第一个资产的描述
        UpdateWindowTitle();                      // 更新窗口标题
    }
}

/**
 * 获取当前资产的显示文本
 * 包含资产名称和编辑进度 批量（如：正在编辑: MyAsset (5/5)）/序列（如：正在编辑: MyAsset (1/5)）
 */
FText SAssetDescriptionWindow::GetCurrentAssetText() const
{
    if (CurrentAssetIndex >= 0 && CurrentAssetIndex < SelectedAssets.Num())
    {
        const FAssetData& AssetData = SelectedAssets[CurrentAssetIndex];

        FString Description = bEnableSequentialEdit ?
            TEXT("正在序列编辑: {0} ({1}/{2})"):TEXT("正在批量编辑: {0} ({1}/{2})")
        ;
        
        FString CurrentAssetText = bEnableSequentialEdit ? 
        AssetData.AssetName.ToString():AssetData.AssetName.ToString() + TEXT("...");
        
        int32 CurrentEditorAsset = bEnableSequentialEdit ? 
        CurrentAssetIndex + 1: SelectedAssets.Num();
        
        return FText::Format(
            FText::FromString(Description),
            FText::FromString(CurrentAssetText),
            FText::AsNumber(CurrentEditorAsset),
            FText::AsNumber(SelectedAssets.Num())
        );
    }
    return FText::GetEmpty();
}

/**
 * 检查是否可以导航到下一个资产
 */
bool SAssetDescriptionWindow::CanNavigateNext() const
{
    return CurrentAssetIndex < SelectedAssets.Num() - 1;
}

/**
 * 检查是否可以导航到上一个资产
 */
bool SAssetDescriptionWindow::CanNavigatePrevious() const
{
    return CurrentAssetIndex > 0;
}
/**
 * 获取当前资产的BlueprintDescription
 * 如果是蓝图资产则返回其描述，否则返回空文本
 */
FText SAssetDescriptionWindow::GetCurrentBlueprintDescription() const
{
    if (CurrentAssetIndex >= 0 && CurrentAssetIndex < SelectedAssets.Num())
    {
        const FAssetData& AssetData = SelectedAssets[CurrentAssetIndex];
        if (AssetData.IsValid())
        {
            if (UObject* Asset = AssetData.GetAsset())
            {
                FString Description;
                
                AssetData.GetTagValue(KeyName, Description);
                
                return FText::FromString(Description);
            }
        }
    }
    return FText::FromString(TEXT("无描述"));
}

/**
 * 保存当前资产的描述
 * @return bool - 保存是否成功
 */
bool SAssetDescriptionWindow::SaveCurrentAssetDescription()
{
    // 检查索引是否有效
    if (CurrentAssetIndex >= 0 && CurrentAssetIndex < SelectedAssets.Num())
    {
        FAssetData& AssetData = SelectedAssets[CurrentAssetIndex];
        if (AssetData.IsValid())
        {
            FString NewDescription = GetCurrentDescription();

            UAssetDescriptionManager::GetInstance()->SetAssetDescription
                (AssetData, NewDescription);
            return true;
        }
    }
    return false;
}

/**
 * 加载指定索引资产的描述
 * @param AssetIndex - 要加载的资产索引
 */
void SAssetDescriptionWindow::LoadAssetDescription(int32 AssetIndex)
{
    if (AssetIndex >= 0 && AssetIndex < SelectedAssets.Num())
    {
        FAssetData& AssetData = SelectedAssets[AssetIndex];
        if (AssetData.IsValid())
        {
            if (UObject* Asset = AssetData.GetAsset())
            {
                ;
                DescriptionTextBox->SetText(FText::FromString(
                    UAssetDescriptionManager::GetInstance()->GetAssetDescription
                  (AssetData)));
            }
        }
    }
}


/**
 * 更新窗口标题
 * 在序列模式下显示当前资产信息，在批量模式下显示固定标题
 */
void SAssetDescriptionWindow::UpdateWindowTitle()
{
    if (TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared()))
    {
        if (bEnableSequentialEdit)
        {
            ParentWindow->SetTitle(GetCurrentAssetText());
        }
        else
        {
            ParentWindow->SetTitle(FText::FromString(TEXT("编辑资产描述")));
        }
    }
}
