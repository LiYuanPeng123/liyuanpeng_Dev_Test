#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerActionLibrary.generated.h"

class UAction_DataAsset;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnActionCompled, AActor*, OutActor, AController*, OutController);

UCLASS()
class CANCERACTIONCREATOR_API UCancerActionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static void LoadActionData(TSoftObjectPtr<UAction_DataAsset> Data, AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "Action",meta=(WorldContext="WorldContextObject"))
	static AActor* SpawnActorFromActionData(const UObject* WorldContextObject, TSoftObjectPtr<UAction_DataAsset> Data ,FTransform SpawnTrans);
	
	UFUNCTION(BlueprintCallable, Category = "AICreator", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm="OnCompleted"))
	static  APawn* K2_CreatePawnFromActionData(UObject* WorldContextObject,
		UAction_DataAsset* ActionData, const FTransform& SpawnTransform,FOnActionCompled OnCompleted,
		ESpawnActorCollisionHandlingMethod CollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn ,
		AActor* InOwner = nullptr);
};
