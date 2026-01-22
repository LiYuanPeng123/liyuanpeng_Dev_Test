#include "ComboGraphFactory.h"
#include "CancerAbility/Public/Combo/ComboGraph.h"

UComboGraphFactory::UComboGraphFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UComboGraph::StaticClass();
}

UObject* UComboGraphFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UComboGraph>(InParent, Class, Name, Flags | RF_Transactional);
}

bool UComboGraphFactory::ShouldShowInNewMenu() const
{
	return true;
}

