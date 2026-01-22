
#pragma once

#include "Modules/ModuleManager.h"

class FCancerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
