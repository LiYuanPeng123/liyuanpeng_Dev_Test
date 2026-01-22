#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "ComboGraphFactory.generated.h"

class UComboGraph;

UCLASS()
class CANCERABILITYEDITOR_API UComboGraphFactory : public UFactory
{
	GENERATED_BODY()
public:
	UComboGraphFactory();
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};

