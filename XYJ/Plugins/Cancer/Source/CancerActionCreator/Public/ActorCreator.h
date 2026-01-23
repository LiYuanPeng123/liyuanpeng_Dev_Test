#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorCreator.generated.h"


class UAction_DataAsset;
/*
 *Actor 生成器
 */
UCLASS()
class CANCERACTIONCREATOR_API AActorCreator : public AActor
{
	GENERATED_BODY()

public:
	AActorCreator(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|PawnData")
	TSoftObjectPtr<UAction_DataAsset> ActorData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview")
	TObjectPtr<USceneComponent> SceneRootComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview")
	TObjectPtr<UChildActorComponent> PreviewComponent;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
