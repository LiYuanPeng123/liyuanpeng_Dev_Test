// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetDescripition/AssetDescription.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetDescripition/CancerAssetDescriptionUserData.h"
#include "CancerAssetEditor.h"
#include "Engine/AssetUserData.h"
#include "Delegates/Delegate.h"
#include "UObject/AssetRegistryTagsContext.h"
#include "UObject/SavePackage.h"

static const TCHAR* DescriptionKey = TEXT("Description");
static FDelegateHandle GExtraTagsHandle;

UAssetDescriptionManager* UAssetDescriptionManager::GetInstance()
{
	static UAssetDescriptionManager* Instance;
	if (!Instance)
	{
		Instance = NewObject<UAssetDescriptionManager>();
		Instance->AddToRoot();
		GExtraTagsHandle = UObject::FAssetRegistryTag::OnGetExtraObjectTagsWithContext.AddStatic(&FCancerAssetEditorModule::HandleAddExtraTags);
	}
	return Instance;
}

FString UAssetDescriptionManager::GetAssetDescription(const FAssetData& AssetData) const
{
	// 1. 优先从 Tag 读取（无加载）
	FString Description;
	if (AssetData.GetTagValue(TEXT("Description"), Description))
	{
		return Description;
	}
	// 2. 再从 MetaData 读取（可能需要加载包）
	if (UPackage* Package = AssetData.GetPackage())
	{
		if (FMetaData* MetaData = &Package->GetMetaData())
		{
			if (UObject* Asset = AssetData.GetAsset())
			{
				FString MetaDescription = MetaData->GetValue(Asset, DescriptionKey);
				if (!MetaDescription.IsEmpty())
				{
					return MetaDescription;
				}
			}
		}
	}
	
	/*if (!AssetData.IsValid())
	{
		return Description;
	}
	if (UPackage* Pack = AssetData.GetPackage())
	{
		if (FMetaData* MetaData = &Pack->GetMetaData())
		{
			Description = MetaData->GetValue(AssetData.GetAsset(),DescriptionKey);
		}
	}*/
	return Description;
}

void UAssetDescriptionManager::SetAssetDescription(const FAssetData& AssetData, const FString& Description)
{
	/*if (FMetaData* MetaData = &AssetData.GetPackage()->GetMetaData())
	{
		MetaData->SetValue(AssetData.GetAsset(),DescriptionKey,*Description);
	}*/

	UPackage* Package = AssetData.GetPackage();
	if (!Package) return;
	
	FMetaData* MetaData = &Package->GetMetaData();
	if (!MetaData) return;
	
	UObject* Asset = AssetData.GetAsset();
	if (!Asset) return;
	
	// 1. 存到 MetaData（持久化）
	MetaData->SetValue(Asset, DescriptionKey, *Description);

	// 2. 标记 Dirty
	Package->MarkPackageDirty();
	if (IInterface_AssetUserData* AssetUserDataInterface = Cast<IInterface_AssetUserData>(Asset))
	{
		UAssetUserData* BaseUserData = AssetUserDataInterface->GetAssetUserDataOfClass(UCancerAssetDescriptionUserData::StaticClass());
		UCancerAssetDescriptionUserData* DescUserData = Cast<UCancerAssetDescriptionUserData>(BaseUserData);
		if (!DescUserData)
		{
			DescUserData = NewObject<UCancerAssetDescriptionUserData>(Asset);
			AssetUserDataInterface->AddAssetUserData(DescUserData);
		}
		DescUserData->Description = Description;
		Asset->Modify();
	}

	/*// 3. 更新 Asset Registry Tag
	{
		TArray<FAssetRegistryTag> Tags;
		Asset->GetAssetRegistryTags(Tags);

		// 查找是否已有 Description 标签
		FAssetRegistryTag* ExistingTag = Tags.FindByPredicate([](const FAssetRegistryTag& Tag)
		{
			return Tag.Name == TEXT("Description");
		});

		if (ExistingTag)
		{
			ExistingTag->Value = Description;
		}
		else
		{
			Tags.Add(FAssetRegistryTag(TEXT("Description"), Description, FAssetRegistryTag::TT_Alphabetical));
		}

		// 关键：要让 Asset Registry 重新获取这些 Tag
		Asset->Modify();
	}*/

	// 3. 强制 Asset Registry 重新扫描该文件
	FString PackageFilePath = FPackageName::LongPackageNameToFilename(
		Package->GetName(), 
		FPackageName::GetAssetPackageExtension()
	);

	TArray<FString> FilesToScan;
	FilesToScan.Add(PackageFilePath);

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().ScanModifiedAssetFiles(FilesToScan);
	
	
}
