
#pragma once

#include "Modules/ModuleManager.h"

class FCancerPreLoadScreen;

class FCancerStartupLoadingScreenModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	bool IsGameModule() const override;

private:
	void OnPreLoadScreenManagerCleanUp();

	TSharedPtr<FCancerPreLoadScreen> PreLoadingScreen;
};


