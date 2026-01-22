// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetPrefixSuffix/AssetPrefixSuffixAction.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "AssetPrefixSuffix/AssetPrefixSuffixWindow.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

void UAssetPrefixSuffixAction::OpenPrefixSuffixWindow()
{
	// 创建窗口
	MainEditorWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("资产前缀/后缀编辑器")))
		.ClientSize(FVector2D(400, 300))
		.SizingRule(ESizingRule::UserSized)
		.MinWidth(300.0f)
		.MinHeight(250.0f);

	// 创建窗口内容
	TSharedRef<SAssetPrefixSuffixWindow> PrefixSuffixWindow = SNew(SAssetPrefixSuffixWindow);
    
	// 设置窗口内容
	MainEditorWindow->SetContent(PrefixSuffixWindow);
    
	// 显示窗口
	TSharedPtr<SWindow> ParentWindow = FGlobalTabmanager::Get()->GetRootWindow();
	if (ParentWindow.IsValid())
	{
		FSlateApplication::Get().AddModalWindow(MainEditorWindow.ToSharedRef(), ParentWindow.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(MainEditorWindow.ToSharedRef());
	}
}

void UAssetPrefixSuffixAction::AddPrefixToSelectedAssets(const FString& Prefix, bool bSkipExisting)
{
	// 获取内容浏览器模块
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    
	// 获取选中的资产
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
    
	// 显示预览窗口
	PreviewChanges(SelectedAssets, Prefix, TEXT(""), true);
}

void UAssetPrefixSuffixAction::AddSuffixToSelectedAssets(const FString& Suffix, bool bSkipExisting)
{
	// 获取内容浏览器模块
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    
	// 获取选中的资产
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
    
	// 显示预览窗口
	PreviewChanges(SelectedAssets, TEXT(""), Suffix, true);
}

void UAssetPrefixSuffixAction::RemovePrefixFromSelectedAssets(const FString& Prefix, bool bOnlyExact)
{
	// 获取内容浏览器模块
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    
	// 获取选中的资产
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
    
	// 显示预览窗口
	PreviewChanges(SelectedAssets, Prefix, TEXT(""), false);
}

void UAssetPrefixSuffixAction::RemoveSuffixFromSelectedAssets(const FString& Suffix, bool bOnlyExact)
{
	// 获取内容浏览器模块
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    
	// 获取选中的资产
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
    
	// 显示预览窗口
	PreviewChanges(SelectedAssets, TEXT(""), Suffix, false);
}

void UAssetPrefixSuffixAction::PreviewChanges(const TArray<FAssetData>& Assets, const FString& Prefix,
	const FString& Suffix, bool bIsAdding)
{
	 // 检查是否有选中的资产
    if (Assets.Num() == 0)
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("请先选择要操作的资产")));
        return;
    }

    // 创建预览窗口
    TSharedRef<SWindow> PreviewWindow = SNew(SWindow)
        .Title(FText::FromString(TEXT("预览修改")))
        .ClientSize(FVector2D(800, 500))
        .SizingRule(ESizingRule::UserSized)
        .MinWidth(600.0f)
        .MinHeight(400.0f);

    TSharedRef<SVerticalBox> ContentBox = SNew(SVerticalBox);

    // 添加标题和说明
    ContentBox->AddSlot()
    .AutoHeight()
    .Padding(5)
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(2)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("以下是即将进行的修改：")))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(2)
        [
            SNew(STextBlock)
            .Text(bIsAdding ? 
                FText::Format(
                    FText::FromString(TEXT("操作类型：添加{0}{1}")),
                    !Prefix.IsEmpty() ? FText::Format(FText::FromString(TEXT("前缀 \"{0}\"")), FText::FromString(Prefix)) : FText::GetEmpty(),
                    !Suffix.IsEmpty() ? FText::Format(FText::FromString(TEXT("后缀 \"{0}\"")), FText::FromString(Suffix)) : FText::GetEmpty()
                ) :
                FText::Format(
                    FText::FromString(TEXT("操作类型：移除{0}{1}")),
                    !Prefix.IsEmpty() ? FText::Format(FText::FromString(TEXT("前缀 \"{0}\"")), FText::FromString(Prefix)) : FText::GetEmpty(),
                    !Suffix.IsEmpty() ? FText::Format(FText::FromString(TEXT("后缀 \"{0}\"")), FText::FromString(Suffix)) : FText::GetEmpty()
                )
            )
            .ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.5f, 0.0f)))
        ]
    ];

    // 添加滚动框
    TSharedRef<SScrollBox> ScrollBox = SNew(SScrollBox);

    // 添加表头
    ScrollBox->AddSlot()
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .FillWidth(0.1f)
        .Padding(5)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("状态")))
        ]
        + SHorizontalBox::Slot()
        .FillWidth(0.45f)
        .Padding(5)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("原始名称")))
        ]
        + SHorizontalBox::Slot()
        .FillWidth(0.45f)
        .Padding(5)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("修改后名称")))
        ]
    ];

    // 添加分隔线
    ScrollBox->AddSlot()
    [
        SNew(SBorder)
        .Padding(FMargin(0, 2))
        .BorderImage(FAppStyle::Get().GetBrush("Menu.Separator"))
    ];

    // 用于统计
    int32 ValidCount = 0;
    int32 InvalidCount = 0;
    int32 NoChangeCount = 0;

    // 用于存储预览结果
    struct FPreviewResult
    {
        TArray<FAssetData> ValidAssets;  // 有效的资产
        TArray<FString> NewNames;        // 对应的新名称
    };
    TSharedPtr<FPreviewResult> PreviewResult = MakeShared<FPreviewResult>();

    // 添加每个资产的预览
    for (const FAssetData& Asset : Assets)
    {
        FString OriginalName = Asset.AssetName.ToString();
        FString NewName;
        FString StatusText;
        FLinearColor StatusColor;
        
        if (bIsAdding)
        {
            NewName = Prefix + OriginalName + Suffix;
            if (NewName == OriginalName)
            {
                StatusText = TEXT("无变化");
                StatusColor = FLinearColor(1.0f, 1.0f, 0.0f);
                NoChangeCount++;
            }
            else
            {
                StatusText = TEXT("有效");
                StatusColor = FLinearColor(0.0f, 1.0f, 0.0f);
                ValidCount++;
                PreviewResult->ValidAssets.Add(Asset);
                PreviewResult->NewNames.Add(NewName);
            }
        }
        else
        {
            NewName = OriginalName;
            bool hasChanged = false;
            
            if (!Prefix.IsEmpty())
            {
                // 检查是否有前缀
                if (HasPrefix(NewName, Prefix))
                {
                    NewName = NewName.RightChop(Prefix.Len());
                    hasChanged = true;
                    StatusText = TEXT("有效");
                    StatusColor = FLinearColor(0.0f, 1.0f, 0.0f);
                    ValidCount++;
                    PreviewResult->ValidAssets.Add(Asset);
                    PreviewResult->NewNames.Add(NewName);
                }
                else
                {
                    StatusText = TEXT("无此前缀");
                    StatusColor = FLinearColor(1.0f, 1.0f, 0.0f);
                    NoChangeCount++;
                }
            }
            
            if (!Suffix.IsEmpty() && !hasChanged)
            {
                // 检查是否有后缀
                if (HasSuffix(NewName, Suffix))
                {
                    NewName = NewName.Left(NewName.Len() - Suffix.Len());
                    hasChanged = true;
                    StatusText = TEXT("有效");
                    StatusColor = FLinearColor(0.0f, 1.0f, 0.0f);
                    ValidCount++;
                    PreviewResult->ValidAssets.Add(Asset);
                    PreviewResult->NewNames.Add(NewName);
                }
                else
                {
                    StatusText = TEXT("无此后缀");
                    StatusColor = FLinearColor(1.0f, 1.0f, 0.0f);
                    NoChangeCount++;
                }
            }
        }

        ScrollBox->AddSlot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(0.1f)
            .Padding(5)
            [
                SNew(STextBlock)
                .Text(FText::FromString(StatusText))
                .ColorAndOpacity(StatusColor)
            ]
            + SHorizontalBox::Slot()
            .FillWidth(0.45f)
            .Padding(5)
            [
                SNew(STextBlock)
                .Text(FText::FromString(OriginalName))
            ]
            + SHorizontalBox::Slot()
            .FillWidth(0.45f)
            .Padding(5)
            [
                SNew(STextBlock)
                .Text(FText::FromString(NewName))
            ]
        ];
    }

    // 添加统计信息
    ContentBox->AddSlot()
    .AutoHeight()
    .Padding(5)
    [
        SNew(STextBlock)
        .Text(FText::Format(FText::FromString(TEXT("统计信息：\n有效：{0} 个\n无效：{1} 个\n无变化：{2} 个")),
            FText::AsNumber(ValidCount),
            FText::AsNumber(InvalidCount),
            FText::AsNumber(NoChangeCount)))
    ];

    ContentBox->AddSlot()
    .FillHeight(1.0f)
    .Padding(5)
    [
        ScrollBox
    ];

    // 添加按钮
    bool bCanProceed = (ValidCount > 0);
    
    ContentBox->AddSlot()
    .AutoHeight()
    .Padding(5)
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        .Padding(5)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("确认")))
            .IsEnabled(bCanProceed)
            .OnClicked_Lambda([this, PreviewWindow, PreviewResult]()
            {
                // 执行重命名操作
                int32 SuccessCount = 0;
                int32 FailedCount = 0;

                for (int32 i = 0; i < PreviewResult->ValidAssets.Num(); ++i)
                {
                    if (RenameAsset(PreviewResult->ValidAssets[i], PreviewResult->NewNames[i]))
                    {
                        SuccessCount++;
                    }
                    else
                    {
                        FailedCount++;
                    }
                }

                // 显示操作结果
                FText Message = FText::Format(
                    FText::FromString(TEXT("重命名操作完成:\n成功: {0}\n失败: {1}")),
                    FText::AsNumber(SuccessCount),
                    FText::AsNumber(FailedCount)
                );

                if (FailedCount > 0)
                {
                    FMessageDialog::Open(EAppMsgType::Ok, Message);
                }
                else
                {
                    FNotificationInfo Info(Message);
                    Info.ExpireDuration = 5.0f;
                    FSlateNotificationManager::Get().AddNotification(Info);
                }

                // 关闭预览窗口
                PreviewWindow->RequestDestroyWindow();

                // 关闭主编辑窗口
                if (TSharedPtr<SWindow> ParentWindow = PreviewWindow->GetParentWindow())
                {
                    TArray<TSharedRef<SWindow>> ChildWindows;
                    FSlateApplication::Get().GetAllVisibleWindowsOrdered(ChildWindows);
                    
                    for (const TSharedRef<SWindow>& Window : ChildWindows)
                    {
                        if (Window->GetTitle().ToString() == TEXT("资产前缀/后缀编辑器"))
                        {
                            Window->RequestDestroyWindow();
                            break;
                        }
                    }
                }

                return FReply::Handled();
            })
        ]
        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        .Padding(5)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("取消")))
            .OnClicked_Lambda([PreviewWindow]()
            {
                // 关闭预览窗口
                PreviewWindow->RequestDestroyWindow();
                return FReply::Handled();
            })
        ]
    ];

    PreviewWindow->SetContent(ContentBox);

    TSharedPtr<SWindow> ParentWindow = FGlobalTabmanager::Get()->GetRootWindow();
    if (ParentWindow.IsValid())
    {
        FSlateApplication::Get().AddModalWindow(PreviewWindow, ParentWindow.ToSharedRef());
    }
    else
    {
        FSlateApplication::Get().AddWindow(PreviewWindow);
    }
}

bool UAssetPrefixSuffixAction::HasPrefix(const FString& AssetName, const FString& Prefix, bool bExactMatch) const
{
	if (bExactMatch)
	{
		// 完全匹配：检查资产名称是否以指定前缀开头
		return AssetName.StartsWith(Prefix);
	}
	else
	{
		// 部分匹配：检查资产名称中是否包含前缀（在开头位置）
		int32 Index = AssetName.Find(Prefix);
		return Index == 0;
	}
}

bool UAssetPrefixSuffixAction::HasSuffix(const FString& AssetName, const FString& Suffix, bool bExactMatch) const
{
	if (bExactMatch)
	{
		// 完全匹配：检查资产名称是否以指定后缀结尾
		return AssetName.EndsWith(Suffix);
	}
	else
	{
		// 部分匹配：检查资产名称中是否包含后缀（在结尾位置）
		int32 Index = AssetName.Find(Suffix);
		return Index != INDEX_NONE && (Index + Suffix.Len() == AssetName.Len());
	}
}

bool UAssetPrefixSuffixAction::RenameAsset(const FAssetData& Asset, const FString& NewName) const
{
	// 验证新名称的有效性
	FString SanitizedName = NewName;
	// 移除无效字符
	for (int32 i = 0; i < SanitizedName.Len(); ++i)
	{
		if (!FChar::IsAlnum(SanitizedName[i]) && SanitizedName[i] != '_')
		{
			SanitizedName.RemoveAt(i);
			--i;
		}
	}

	// 确保名称不为空且以字母开头
	if (SanitizedName.IsEmpty() || !FChar::IsAlpha(SanitizedName[0]))
	{
		FText ErrorMessage = FText::Format(
			FText::FromString(TEXT("无效的资产名称: {0}\n名称必须以字母开头且只能包含字母、数字和下划线")),
			FText::FromString(NewName)
		);
		FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
		return false;
	}

	// 获取资产的完整路径
	FString PackagePath = Asset.PackagePath.ToString();
	if (PackagePath.IsEmpty())
	{
		PackagePath = FPackageName::GetLongPackagePath(Asset.PackageName.ToString());
	}

	// 确保路径以 / 开头
	if (!PackagePath.StartsWith(TEXT("/")))
	{
		PackagePath = TEXT("/") + PackagePath;
	}

	// 检查新名称是否已被使用
	FString NewPackageName = PackagePath + TEXT("/") + SanitizedName;
	if (FPackageName::DoesPackageExist(NewPackageName))
	{
		FText ErrorMessage = FText::Format(
			FText::FromString(TEXT("资产名称已存在: {0}")),
			FText::FromString(SanitizedName)
		);
		FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
		return false;
	}

	// 使用 AssetTools 重命名资产
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	TArray<FAssetRenameData> RenameData;
	FAssetRenameData& NewRenameData = RenameData.AddDefaulted_GetRef();
	NewRenameData.Asset = Asset.GetAsset();  // 这里会自动处理资产的加载
	NewRenameData.NewPackagePath = PackagePath;
	NewRenameData.NewName = SanitizedName;

	return AssetToolsModule.Get().RenameAssets(RenameData);
}
