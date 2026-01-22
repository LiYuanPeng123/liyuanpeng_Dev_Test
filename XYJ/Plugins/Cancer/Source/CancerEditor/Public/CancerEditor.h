// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUICommandList;

class FCancerEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenus();
	void OnEditRedirectTagClicked();
	TSharedRef<class SDockTab> SpawnRedirectTagTab(const class FSpawnTabArgs& Args);
	
	TSharedPtr<FUICommandList> PluginCommands;
};
