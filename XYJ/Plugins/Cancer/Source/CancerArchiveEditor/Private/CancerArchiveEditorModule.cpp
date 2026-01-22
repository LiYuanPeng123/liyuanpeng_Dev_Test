#include "CancerArchiveEditor.h"
#include "K2Node_LoadStructFromArchive.h"

#define LOCTEXT_NAMESPACE "FCancerArchiveEditorModule"

void FCancerArchiveEditorModule::StartupModule()
{
	
}

void FCancerArchiveEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCancerArchiveEditorModule, CancerArchiveEditor)
