
#include "CancerAnimationEditor.h"
#include "Customization/LocomotionDirectionCustomization.h"
#include "Types/AnimInstanceDefinition.h"

#define LOCTEXT_NAMESPACE "FCancerAnimationEditorModule"

void FCancerAnimationEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(FLocomotionDirection::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FLocomotionDirectionCustomization::MakeInstance));
}

void FCancerAnimationEditorModule::ShutdownModule()
{
	FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
	if (PropertyModule)
	{
		PropertyModule->UnregisterCustomPropertyTypeLayout(FLocomotionDirection::StaticStruct()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCancerAnimationEditorModule, CancerAnimationEditor)