#pragma once
#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "ItemDefinitionFactory.generated.h"


UCLASS()
class CANCERINVENTORYEDITOR_API UItemDefinitionFactory : public UFactory
{
	GENERATED_BODY()
public:
	UItemDefinitionFactory();
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
									  UObject* Context, FFeedbackContext* Warn) override;
};
