
#include "Factorys/EquipmentDefinitionFactory.h"

#include "Equipment/EquipmentDefinition.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Engine/Blueprint.h"

UEquipmentDefinitionFactory::UEquipmentDefinitionFactory()
{
	bCreateNew = true;
	SupportedClass = UEquipmentDefinition::StaticClass();
}

UObject* UEquipmentDefinitionFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                            UObject* Context, FFeedbackContext* Warn)
{
	return FKismetEditorUtilities::CreateBlueprint(UEquipmentDefinition::StaticClass(), InParent, InName, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), NAME_None);
}
