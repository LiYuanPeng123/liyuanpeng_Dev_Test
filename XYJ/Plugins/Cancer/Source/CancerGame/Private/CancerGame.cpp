// Copyright Epic Games, Inc. All Rights Reserved.

#include "CancerGame.h"

#include "GameplayTagsManager.h"

#define LOCTEXT_NAMESPACE "FCancerGameModule"

void FCancerGameModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	UGameplayTagsManager::Get().LoadGameplayTagTables();   // 强制重载所有 ini
}

void FCancerGameModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCancerGameModule, CancerGame)