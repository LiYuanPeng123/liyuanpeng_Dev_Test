
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerNumPopFunctionLibrary.generated.h"

class UCancerNumberPopComponent;
/**
 * 
 */
UCLASS()
class CANCERNUMBERPOP_API UCancerNumPopFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
#pragma  region 获取UCancerNumberPopComponent
	template <class T = UCancerNumberPopComponent>
	static T* GetCancerNumPopComponent(AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;
		return Actor->GetComponentByClass<T>();
	};

	UFUNCTION(BlueprintCallable, Category = "Cancer", DisplayName="Get CancerNumberPopComponent",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static UCancerNumberPopComponent* K2_GetCancerStaggerComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerNumberPopComponent> OutputClass = nullptr);
#pragma endregion 
};
