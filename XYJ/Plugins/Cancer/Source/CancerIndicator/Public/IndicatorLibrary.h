#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "IndicatorLibrary.generated.h"

#define UE_API CANCERINDICATOR_API

class UCancerIndicatorSourceComponent;
class UCancerIndicatorManagerComponent;
class AController;
class UObject;
struct FFrame;

UCLASS(MinimalAPI)
class UIndicatorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UE_API UIndicatorLibrary();

	/**  */
	UFUNCTION(BlueprintCallable, Category = Indicator)
	static UE_API UCancerIndicatorManagerComponent* GetIndicatorManagerComponent(AController* Controller);

	// 获取CancerIndicatorManagerComponent
	template <class T = UCancerIndicatorManagerComponent>
	static T* GetCancerIndicatorManagerComponent(AActor* Actor)
	{
		if (!IsValid(Actor))return nullptr;
		return Actor->FindComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Indicator", DisplayName="GetCancerIndicatorManagerComponent",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerIndicatorManagerComponent* K2_GetCancerIndicatorManagerComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerIndicatorManagerComponent> OutputClass = nullptr);

	// UCancerIndicatorSourceComponent
	template <class T = UCancerIndicatorSourceComponent>
	static T* GetCancerIndicatorSourceComponent(AActor* Actor)
	{
		if (!IsValid(Actor))return nullptr;
		return Actor->FindComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Indicator", DisplayName="GetCancerIndicatorSourceComponent",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerIndicatorSourceComponent* K2_GetCancerIndicatorSourceComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerIndicatorSourceComponent> OutputClass = nullptr);


	
};

#undef UE_API
