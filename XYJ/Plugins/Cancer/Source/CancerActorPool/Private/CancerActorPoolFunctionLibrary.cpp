#include "CancerActorPoolFunctionLibrary.h"

#include "CancerActorPoolComponent.h"

UCancerActorPoolComponent* UCancerActorPoolFunctionLibrary::K2_GetCancerActorPoolComponent(AActor* Actor,
                                                                                           bool& bIsValid, TSubclassOf<UCancerActorPoolComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerActorPoolComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerActorPoolComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}
