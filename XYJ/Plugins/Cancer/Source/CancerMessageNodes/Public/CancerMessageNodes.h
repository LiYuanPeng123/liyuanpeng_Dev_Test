
#pragma once

#include "Modules/ModuleManager.h"

class FCancerMessageNodesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
