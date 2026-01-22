
#pragma once

#include "Modules/ModuleManager.h"

class FCancerUIModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
