#pragma once
#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "EquipmentDefinitionFactory.generated.h"


UCLASS()
class CANCERINVENTORYEDITOR_API UEquipmentDefinitionFactory : public UFactory
{
	GENERATED_BODY()

public:
	UEquipmentDefinitionFactory();

	//~UFactory interface
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override;
	//~End of UFactory interface
};
