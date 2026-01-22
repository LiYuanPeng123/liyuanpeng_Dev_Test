#pragma once

#include "CoreMinimal.h"
#include "CancerIconStyle.h"
#include "ContentBrowserDelegates.h"
#include "UObject/Object.h"
#include "AssetDescription.generated.h"

UCLASS()
class CANCERASSETEDITOR_API UAssetDescriptionManager : public UObject
{
	GENERATED_BODY()

public:
	static UAssetDescriptionManager* GetInstance();

	FString GetAssetDescription(const FAssetData& AssetData) const;

	void SetAssetDescription(const FAssetData& AssetData, const FString& Description);
};


class FAssetDescriptionStateGenerator
{
public:
	static FString GetDescriptionFromMeta(const FAssetData& AssetData)
	{
		if (UPackage* Package = AssetData.GetPackage())
		{
			FMetaData& MetaData = Package->GetMetaData();

			if (const UObject* Asset = AssetData.GetAsset())
			{
				static const FName DescriptionKey(TEXT("Description"));

				if (MetaData.HasValue(Asset, DescriptionKey))
				{
					return MetaData.GetValue(Asset, DescriptionKey);
				}
			}
		}
		return FString();
	}

	// 生成图标
	static TSharedRef<SWidget> GenerateIcon(const FAssetData& AssetData)
	{
		if (UAssetDescriptionManager* Manager = UAssetDescriptionManager::GetInstance())
		{
			FString Description = GetDescriptionFromMeta(AssetData);
			/*if (!AssetData.PackageName.IsNone())
			{
			    if (UPackage* Package = FindPackage(NULL, *AssetData.PackageName.ToString()))
			    {*/
			if (!Description.IsEmpty())
			{
				FString DisplayText = Description;
				if (DisplayText.Len() > 4)
				{
					DisplayText.LeftInline(4, EAllowShrinking::No);
				}
				return SNew(SHorizontalBox)
						// Viewport
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SBox)
							[
								SNew(SImage)
								.Image(FCancerIconStyle::Get().GetBrush("WriteCat"))
								.ColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f))
							]
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString(DisplayText))
							.AutoWrapText(true)
							.ColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
						];
			}
			/*
		}*/
		}
		return SNullWidget::NullWidget;
	}

	// 生成工具提示
	static TSharedRef<SWidget> GenerateToolTip(const FAssetData& AssetData)
	{
		UAssetDescriptionManager* Manager = UAssetDescriptionManager::GetInstance();
		if (Manager)
		{
			FString Description = Manager->GetAssetDescription(AssetData);
			if (!Description.IsEmpty())
			{
				// 创建一个带背景色的边框
				return SNew(SBorder)
				                    .Padding(8.0f) // 内边距
				                    .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.0f))
				                    .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
					[
						SNew(STextBlock)
						.Text(FText::FromString(Description))
						.AutoWrapText(true)
						.ColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
						/*.FontSize(12) // 字体大小*/
					];
			}
		}
		return SNullWidget::NullWidget;
	}

	// 获取委托
	static FOnGenerateAssetViewExtraStateIndicators GetIconGenerator()
	{
		return FOnGenerateAssetViewExtraStateIndicators::CreateStatic(&GenerateIcon);
	}

	static FOnGenerateAssetViewExtraStateIndicators GetToolTipGenerator()
	{
		return FOnGenerateAssetViewExtraStateIndicators::CreateStatic(&GenerateToolTip);
	}
};
