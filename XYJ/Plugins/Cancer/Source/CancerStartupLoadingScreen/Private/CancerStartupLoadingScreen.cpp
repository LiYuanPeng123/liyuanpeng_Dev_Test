// Copyright Epic Games, Inc. All Rights Reserved.

#include "CancerStartupLoadingScreen.h"
#include "CancerPreLoadScreen.h"
#include "PreLoadScreenManager.h"
#define LOCTEXT_NAMESPACE "FCancerStartupLoadingScreenModule"

void FCancerStartupLoadingScreenModule::StartupModule()
{
	// No need to load these assets on dedicated servers.
	// Still want to load them in commandlets so cook catches them
	if (!IsRunningDedicatedServer())
	{
		PreLoadingScreen = MakeShared<FCancerPreLoadScreen>();
		PreLoadingScreen->Init();

		if (!GIsEditor && FApp::CanEverRender() && FPreLoadScreenManager::Get())
		{
			FPreLoadScreenManager::Get()->RegisterPreLoadScreen(PreLoadingScreen);
			FPreLoadScreenManager::Get()->OnPreLoadScreenManagerCleanUp.AddRaw(this, &FCancerStartupLoadingScreenModule::OnPreLoadScreenManagerCleanUp);
		}
	}
}

void FCancerStartupLoadingScreenModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

bool FCancerStartupLoadingScreenModule::IsGameModule() const
{
	return true;
}

void FCancerStartupLoadingScreenModule::OnPreLoadScreenManagerCleanUp()
{
	//Once the PreLoadScreenManager is cleaning up, we can get rid of all our resources too
	PreLoadingScreen.Reset();
	ShutdownModule();
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCancerStartupLoadingScreenModule, CancerStartupLoadingScreen)