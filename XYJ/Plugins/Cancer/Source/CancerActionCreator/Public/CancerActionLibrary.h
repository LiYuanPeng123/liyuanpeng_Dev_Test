#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerActionLibrary.generated.h"

class UAction_DataAsset;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnActionCompled, AActor*, OutActor);

UCLASS()
class CANCERACTIONCREATOR_API UCancerActionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void LoadActionData(TSoftObjectPtr<UAction_DataAsset> Data, AActor* Actor, FOnActionCompled OnCompleted);

	UFUNCTION(BlueprintCallable, Category = "Action", meta=(WorldContext="WorldContextObject"))
	static AActor* SpawnActorFromActionData(const UObject* WorldContextObject, TSoftObjectPtr<UAction_DataAsset> Data,
	                                        FTransform SpawnTrans);

	UFUNCTION(BlueprintCallable, Category = "AICreator",
		meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm="OnCompleted"))
	static AActor* K2_CreatePawnFromActionData(UObject* WorldContextObject,
	                                           TSoftObjectPtr<UAction_DataAsset> Data, const FTransform& SpawnTransform,
	                                           FOnActionCompled OnCompleted,
	                                           ESpawnActorCollisionHandlingMethod CollisionHandlingMethod =
		                                           ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn,
	                                           AActor* InOwner = nullptr);
};
