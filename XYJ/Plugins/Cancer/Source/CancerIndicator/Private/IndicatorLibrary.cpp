#include "IndicatorLibrary.h"
#include "CancerIndicatorManagerComponent.h"
#include "CancerIndicatorSourceComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorLibrary)

class AController;

UIndicatorLibrary::UIndicatorLibrary()
{
}

UCancerIndicatorManagerComponent* UIndicatorLibrary::GetIndicatorManagerComponent(AController* Controller)
{
	return UCancerIndicatorManagerComponent::GetComponent(Controller);
}


class UCancerIndicatorManagerComponent* UIndicatorLibrary::K2_GetCancerIndicatorManagerComponent(AActor* Actor,
                                                                                                 bool& bIsValid, TSubclassOf<UCancerIndicatorManagerComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerIndicatorManagerComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerIndicatorManagerComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

class UCancerIndicatorSourceComponent* UIndicatorLibrary::K2_GetCancerIndicatorSourceComponent(AActor* Actor,
	bool& bIsValid, TSubclassOf<UCancerIndicatorSourceComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerIndicatorSourceComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerIndicatorSourceComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

