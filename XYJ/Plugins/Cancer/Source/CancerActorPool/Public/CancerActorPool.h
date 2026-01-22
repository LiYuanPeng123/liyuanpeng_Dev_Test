
#pragma once

#include "Modules/ModuleManager.h"

class FCancerActorPoolModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
