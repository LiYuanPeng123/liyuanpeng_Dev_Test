#pragma once

#include "CoreMinimal.h"
#include "CancerGameMode.h"
#include "XYJ_GameMode.generated.h"

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

	UFUNCTION()
	void HandPawnFinish(APawn* StartPawn);
	

	UFUNCTION(BlueprintPure, Category="Load")
	float GetWorldLoadProgress() const;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadProgress,float, Progress);

    UPROPERTY(BlueprintAssignable, Category="Load")
    FOnLoadProgress OnPawnLoadProgress;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAction_DataAsset> DefaultPawnData;
	
	UFUNCTION(BlueprintPure, Category="Pawn")
	UAction_DataAsset* GetEffectivePawnData() const;

private:
    TMap<TWeakObjectPtr<APawn>, TSharedPtr<struct FStreamableHandle>> PawnLoadHandles;
    float LastProgress = 0.f;
    bool bLoadCompleted = false;
};
