#include "Factorys/ItemDefinitionFactory.h"
#include "InventoryItemDefinition.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Engine/Blueprint.h"

UItemDefinitionFactory::UItemDefinitionFactory()
{
	bCreateNew = true;
	SupportedClass = UInventoryItemDefinition::StaticClass();
}

UObject* UItemDefinitionFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                 UObject* Context, FFeedbackContext* Warn)
{
	return FKismetEditorUtilities::CreateBlueprint(UInventoryItemDefinition::StaticClass(), InParent, InName, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), NAME_None);
}
