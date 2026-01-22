
#include "CancerArchive.h"

#define LOCTEXT_NAMESPACE "FCancerArchiveModule"
void FCancerArchiveModule::StartupModule()
{
	IModuleInterface::StartupModule();
}

void FCancerArchiveModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCancerArchiveModule, CancerArchive)