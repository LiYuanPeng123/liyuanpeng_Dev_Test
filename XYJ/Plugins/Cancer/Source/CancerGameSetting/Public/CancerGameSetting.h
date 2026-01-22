
#pragma once

#include "Modules/ModuleManager.h"

class FCancerGameSettingModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
