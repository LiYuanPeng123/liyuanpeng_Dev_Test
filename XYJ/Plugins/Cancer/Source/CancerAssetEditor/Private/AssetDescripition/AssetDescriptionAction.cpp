// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetDescripition/AssetDescriptionAction.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "AssetDescripition/SAssetDescriptionWindow.h"

/**
 * 批量编辑选中资产的描述
 * 此函数会为所有选中的资产打开一个统一的描述编辑窗口
 * 在这个模式下，所有资产会被设置为相同的描述
 */
void UAssetDescriptionAction::EditSelectedAssetsDescription()
{
	// 加载并获取内容浏览器模块的引用
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    
	// 获取当前在内容浏览器中选中的资产列表
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);

    
    
	// 只有当有资产被选中时才创建窗口
	if (SelectedAssets.Num() > 0)
	{
		// 创建一个新的Slate窗口
		TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(FText::FromString(TEXT("编辑资产描述")))      // 设置窗口标题
			.ClientSize(FVector2D(400, 300))                    // 设置窗口初始大小
			.SizingRule(ESizingRule::UserSized)                // 允许用户调整窗口大小
			.MinWidth(300.0f)                                  // 设置最小宽度
			.MinHeight(200.0f);                                // 设置最小高度

		// 创建资产描述编辑器窗口实例
		TSharedRef<SAssetDescriptionWindow> DescriptionWindow = SNew(SAssetDescriptionWindow)
			.bEnableSequentialEdit(false);                     // 设置为批量编辑模式
        
		// 设置要编辑的资产列表
		DescriptionWindow->SetAssetDescriptions(SelectedAssets);
        
		// 将描述编辑器设置为窗口的内容
		Window->SetContent(DescriptionWindow);
        
		// 获取主窗口并显示编辑器窗口
		TSharedPtr<SWindow> ParentWindow = FGlobalTabmanager::Get()->GetRootWindow();
		if (ParentWindow.IsValid())
		{
			// 如果有主窗口，创建为模态窗口（阻止与其他窗口交互）
			FSlateApplication::Get().AddModalWindow(Window, ParentWindow.ToSharedRef());
		}
		else
		{
			// 如果没有主窗口，创建为普通窗口
			FSlateApplication::Get().AddWindow(Window);
		}
	}
}

/**
 * 逐个编辑选中资产的描述
 * 此函数会打开一个支持逐个编辑的窗口
 * 用户可以通过上一个/下一个按钮在资产之间切换
 * 每个资产可以设置不同的描述
 */
void UAssetDescriptionAction::EditSelectedAssetsDescriptionSequentially()
{
	// 加载并获取内容浏览器模块的引用
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    
	// 获取当前在内容浏览器中选中的资产列表
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
    
	// 只有当有资产被选中时才创建窗口
	if (SelectedAssets.Num() > 0)
	{
		// 创建一个新的Slate窗口
		TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(FText::FromString(TEXT("逐个编辑资产描述")))  // 设置窗口标题
			.ClientSize(FVector2D(500, 400))                    // 设置窗口初始大小
			.SizingRule(ESizingRule::UserSized)                // 允许用户调整窗口大小
			.MinWidth(400.0f)                                  // 设置最小宽度
			.MinHeight(300.0f);                                // 设置最小高度

		// 创建资产描述编辑器窗口实例
		TSharedRef<SAssetDescriptionWindow> DescriptionWindow = SNew(SAssetDescriptionWindow)
			.bEnableSequentialEdit(true);                      // 设置为序列编辑模式
        
		// 设置要编辑的资产列表
		DescriptionWindow->SetAssetDescriptions(SelectedAssets);
        
		// 将描述编辑器设置为窗口的内容
		Window->SetContent(DescriptionWindow);
        
		// 获取主窗口并显示编辑器窗口
		TSharedPtr<SWindow> ParentWindow = FGlobalTabmanager::Get()->GetRootWindow();
		if (ParentWindow.IsValid())
		{
			// 如果有主窗口，创建为模态窗口（阻止与其他窗口交互）
			FSlateApplication::Get().AddModalWindow(Window, ParentWindow.ToSharedRef());
		}
		else
		{
			// 如果没有主窗口，创建为普通窗口
			FSlateApplication::Get().AddWindow(Window);
		}
	}
}
