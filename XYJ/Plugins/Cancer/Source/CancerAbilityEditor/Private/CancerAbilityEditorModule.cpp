#include "CancerAbilityEditorModule.h"
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "AssetTypeActions_ComboGraph.h"

IMPLEMENT_MODULE(FCancerAbilityEditorModule, CancerAbilityEditor);

void FCancerAbilityEditorModule::StartupModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
		ComboGraphAssetTypeActions = MakeShareable(new FAssetTypeActions_ComboGraph());
		AssetTools.RegisterAssetTypeActions(ComboGraphAssetTypeActions.ToSharedRef());
	}
}

void FCancerAbilityEditorModule::ShutdownModule()
{
	if (ComboGraphAssetTypeActions.IsValid() && FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
		AssetTools.UnregisterAssetTypeActions(ComboGraphAssetTypeActions.ToSharedRef());
		ComboGraphAssetTypeActions.Reset();
	}
}

