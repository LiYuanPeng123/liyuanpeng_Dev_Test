

#include "Equipment/EquipmentDefinition.h"

#include "Equipment/EquipmentInstance.h"


UEquipmentDefinition::UEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	InstanceType = UEquipmentInstance::StaticClass();
}
