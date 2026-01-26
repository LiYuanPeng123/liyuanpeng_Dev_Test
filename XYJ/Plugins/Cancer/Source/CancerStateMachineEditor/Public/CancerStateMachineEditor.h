#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "AssetTypeCategories.h"

class FCancerStateMachineEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

	EAssetTypeCategories::Type GetAssetCategory() const { return CancerAssetCategory; }

private:
	TArray<TSharedPtr<class IAssetTypeActions>> RegisteredAssetTypeActions;
	EAssetTypeCategories::Type CancerAssetCategory;
};
