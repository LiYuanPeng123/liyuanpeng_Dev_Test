#pragma once

#include "CoreMinimal.h"
#include "CancerGameMode.h"
#include "CancertGlobalAbilitySystem.h"
#include "XYJ_GameMode.generated.h"

class UGameplayAbility;
class UAction_DataAsset;
class UPawnData;


/**
 * 
 */
UCLASS()
class XYJ_API AXYJ_GameMode : public ACancerGameMode
{
	GENERATED_BODY()

public:
	AXYJ_GameMode();

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintNativeEvent, Category="Pawn")
	void HandFinish(AActor* Actor);
	
	UFUNCTION()
	void HandPawnFinish(APawn* StartPawn);
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAction_DataAsset> DefaultPawnData;
	UFUNCTION(BlueprintPure, Category="Pawn")
	UAction_DataAsset* GetEffectivePawnData() const;
	UFUNCTION(BlueprintPure, Category="Pawn")
	TSoftObjectPtr<UAction_DataAsset> GetEffectivePawnDataSoft() const;
	
	
};
