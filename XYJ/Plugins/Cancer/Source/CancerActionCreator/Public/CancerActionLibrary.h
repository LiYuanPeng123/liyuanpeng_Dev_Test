#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerActionLibrary.generated.h"

class UAction_DataAsset;

UCLASS()
class CANCERACTIONCREATOR_API UCancerActionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static void LoadActionData(TSoftObjectPtr<UAction_DataAsset> Data, AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "Action",meta=(WorldContext="WorldContextObject"))
	static AActor* SpawnActorFromActionData(const UObject* WorldContextObject, TSoftObjectPtr<UAction_DataAsset> Data ,FTransform SpawnTrans);
};
