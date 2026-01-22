
#include "AssetPrefixSuffix/AssetPrefixSuffixWindow.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "AssetPrefixSuffix/AssetPrefixSuffixAction.h"
#include "Widgets/Layout/SUniformGridPanel.h"

void SAssetPrefixSuffixWindow::Construct(const FArguments& InArgs)
{
	// 获取选中的资产数量
    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    TArray<FAssetData> SelectedAssets;
    ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
    
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(STextBlock)
            .Text(FText::Format(NSLOCTEXT("Super", "SelectedAssetsCount", "已选择 {0} 个资产"), FText::AsNumber(SelectedAssets.Num())))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(2)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(2)
                [
                    SNew(STextBlock)
                    .Text(NSLOCTEXT("Super", "Prefix", "前缀："))
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SAssignNew(PrefixInputBox, SEditableTextBox)
                    .HintText(FText::FromString(TEXT("输入前缀...")))
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(2)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(2)
                [
                    SNew(STextBlock)
                    .Text(NSLOCTEXT("Super", "Suffix", "后缀："))
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SAssignNew(SuffixInputBox, SEditableTextBox)
                    .HintText(FText::FromString(TEXT("输入后缀...")))
                ]
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SUniformGridPanel)
            .SlotPadding(FMargin(2))
            + SUniformGridPanel::Slot(0, 0)
            [
                SNew(SButton)
                .Text(NSLOCTEXT("Super", "Add", "添加"))
                .OnClicked(this, &SAssetPrefixSuffixWindow::OnAddClicked)
            ]
            + SUniformGridPanel::Slot(1, 0)
            [
                SNew(SButton)
                .Text(NSLOCTEXT("Super", "Remove", "移除"))
                .OnClicked(this, &SAssetPrefixSuffixWindow::OnRemoveClicked)
            ]
            + SUniformGridPanel::Slot(2, 0)
            [
                SNew(SButton)
                .Text(NSLOCTEXT("Super", "Cancel", "取消"))
                .OnClicked(this, &SAssetPrefixSuffixWindow::OnCancelClicked)
            ]
        ]
    ];
}

FReply SAssetPrefixSuffixWindow::OnAddClicked()
{
    FString Prefix = PrefixInputBox->GetText().ToString();
    FString Suffix = SuffixInputBox->GetText().ToString();

    if (!Prefix.IsEmpty() || !Suffix.IsEmpty())
    {
        UAssetPrefixSuffixAction* Action = NewObject<UAssetPrefixSuffixAction>();
        if (Action)
        {
            if (!Prefix.IsEmpty())
            {
                Action->AddPrefixToSelectedAssets(Prefix);
            }
            if (!Suffix.IsEmpty())
            {
                Action->AddSuffixToSelectedAssets(Suffix);
            }
        }
    }

    return FReply::Handled();
}

FReply SAssetPrefixSuffixWindow::OnRemoveClicked()
{
    FString Prefix = PrefixInputBox->GetText().ToString();
    FString Suffix = SuffixInputBox->GetText().ToString();

    if (!Prefix.IsEmpty() || !Suffix.IsEmpty())
    {
        UAssetPrefixSuffixAction* Action = NewObject<UAssetPrefixSuffixAction>();
        if (Action)
        {
            if (!Prefix.IsEmpty())
            {
                Action->RemovePrefixFromSelectedAssets(Prefix);
            }
            if (!Suffix.IsEmpty())
            {
                Action->RemoveSuffixFromSelectedAssets(Suffix);
            }
        }
    }

    return FReply::Handled();
}

FReply SAssetPrefixSuffixWindow::OnCancelClicked()
{
    if (TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared()))
    {
        ParentWindow->RequestDestroyWindow();
    }
    return FReply::Handled();
}

