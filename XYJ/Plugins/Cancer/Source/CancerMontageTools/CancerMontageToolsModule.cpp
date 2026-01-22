#include "Modules/ModuleManager.h"

class FCancerMontageToolsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FCancerMontageToolsModule, CancerMontageTools)

