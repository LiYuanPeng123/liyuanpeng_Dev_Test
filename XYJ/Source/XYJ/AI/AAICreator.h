#pragma once

#include "CoreMinimal.h"
#include "Action_DataAssetTemplate.h"
#include "Trigger_Base.h"
#include "Delegates/Delegate.h"
#include "AAICreator.generated.h"

class UAction_DataAsset;
struct FStreamableHandle;
class UPawnData;


UCLASS()
class XYJ_API AAICreator : public ATrigger_Base
{
	GENERATED_BODY()

public:
	AAICreator(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	//进入
	virtual void OnShowTriggerEnter_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp) override;

	//离开
	virtual void OnShowTriggerExit_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp) override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|PawnData")
	TSoftObjectPtr<UAction_DataAsset> PawnData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview")
	TObjectPtr<USceneComponent> SceneRootComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview")
	TObjectPtr<UChildActorComponent> PreviewComponent;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;


	void StartUnloadCheck();

	void StopUnloadCheck();

	UFUNCTION()
	void CheckUnloadCondition();


	UPROPERTY()
	FTimerHandle UnloadCheckTimer;

	UFUNCTION(BlueprintCallable)
	bool IsPlayerControlled(AActor* InActor) const;

	UPROPERTY()
	AActor* CachePawn;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> CachedController;

	UPROPERTY()
	FVector StartLocation;


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
