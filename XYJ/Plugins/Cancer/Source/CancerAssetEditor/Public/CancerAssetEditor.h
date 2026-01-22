

#pragma once

#include "IAssetTypeActions.h"
#include "Modules/ModuleManager.h"
#include "Delegates/Delegate.h"



class FCancerAssetEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static void HandleAddExtraTags(FAssetRegistryTagsContext Context);
	
	protected:
    void RegisterAssetTypeAction(class IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);
    
    /** All created asset type actions. Cached here so that we can unregister it during shutdown. */
    TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
    
    FDelegateHandle ExtraTagsHandle;
	
};
