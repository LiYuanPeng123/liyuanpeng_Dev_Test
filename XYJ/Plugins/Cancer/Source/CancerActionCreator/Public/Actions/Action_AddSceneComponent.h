
#pragma once

#include "CoreMinimal.h"
#include "Action_AddActorComponent.h"
#include "GameplayTagContainer.h"

#include "Action_AddSceneComponent.generated.h"


UCLASS()
class CANCERACTIONCREATOR_API UAction_AddSceneComponent : public UAction_AddActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SceneComponent", DisplayName="父级标签", meta=(Categories="组件标签"))
	FGameplayTag ParentComponentTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SceneComponent", DisplayName="父级插槽", meta=(Categories="组件插槽"))
	FGameplayTag SocketTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SceneComponent", DisplayName = "组件偏移")
	FVector ComponentLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SceneComponent", DisplayName = "组件旋转")
	FRotator ComponentRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SceneComponent", DisplayName = "组件缩放")
	FVector ComponentScale = FVector::OneVector;
	
	virtual void PostExecuteAction_Implementation(AActor* InActor);

	virtual void AttachToParent(AActor* InActor) const;
	UFUNCTION(BlueprintCallable)
	FName GetSocketName() const;
};


