
#include "CancerAssetEditor.h"

#include "AssetToolsModule.h"
#include "CancerIconStyle.h"
#include "ContentBrowserModule.h"
#include "AssetDescripition/AssetDescription.h"
#include "CancerEditor/Public/CancerEditor.h"
#include "UObject/AssetRegistryTagsContext.h"

#define LOCTEXT_NAMESPACE "FCancerAssetEditorModule"

void FCancerAssetEditorModule::StartupModule()
{
	FCancerIconStyle::Get().Initialize();


	// 注册资产分类
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	const EAssetTypeCategories::Type AssetCategory = AssetTools.RegisterAdvancedAssetCategory(
		FName(TEXT("CancerGame")),
		LOCTEXT("CancerGameAssetCategory", "Cancer Game")
	);
    
	// 注册资产视图状态生成器
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    
	// 获取所有生成器
	TArray<FAssetViewExtraStateGenerator>& Generators = const_cast<TArray<FAssetViewExtraStateGenerator>&>(ContentBrowserModule.GetAllAssetViewExtraStateGenerators());
    
	// 添加新的生成器
	Generators.Add(FAssetViewExtraStateGenerator(
		FAssetDescriptionStateGenerator::GetIconGenerator(),
		FAssetDescriptionStateGenerator::GetToolTipGenerator()
	));
    
	//UObject::FAssetRegistryTag::OnGetExtraObjectTagsWithContext.AddStatic(&FCancerAssetEditorModule::HandleAddExtraTags);
}

void FCancerAssetEditorModule::ShutdownModule()
{
	FCancerIconStyle::Get().Shutdown();
	UObject::FAssetRegistryTag::OnGetExtraObjectTagsWithContext.RemoveAll(this);
}

void FCancerAssetEditorModule::HandleAddExtraTags(FAssetRegistryTagsContext Context)
{
	UObject* Object = const_cast<UObject*>(Context.GetObject());
	if (!Object || !Object->IsValidLowLevel())
		return;
    
	UPackage* Pack = Object->GetPackage();
	if (Pack)
	{
		if (FMetaData* MetaData = &Pack->GetMetaData())
		{
			// 示例：为特定类添加标签
			Context.AddTag(UObject::FAssetRegistryTag(
				TEXT("Description"),
			   MetaData->GetValue(Object,TEXT("Description")), 
				UObject::FAssetRegistryTag::TT_Alphabetical
			));
		}
	}
}

void FCancerAssetEditorModule::RegisterAssetTypeAction(class IAssetTools& AssetTools,
	TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCancerAssetEditorModule, CancerAssetEditor)