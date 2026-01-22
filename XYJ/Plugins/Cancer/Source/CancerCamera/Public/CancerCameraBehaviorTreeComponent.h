#pragma once

#include "CoreMinimal.h"
#include "CancerCameraComponent.h"
#include "CancerCameraBehaviorTreeComponent.generated.h"

class UBehaviorTreeComponent;
class UBehaviorTree;
class UBlackboardComponent;

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class CANCERCAMERA_API UCancerCameraBehaviorTreeComponent : public UCancerCameraComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta=(ExposeOnSpawn))
	TObjectPtr<UBehaviorTree> CameraBehaviorTree;

	UFUNCTION(BlueprintCallable)
	void SetCameraBehaviorTree(UBehaviorTree* NewBT);
	void SetDesiredCameraMode(UCancerCameraMode* NewCameraModeTemplate);

	virtual void DrawDebug(UCanvas* Canvas) const override;
	UFUNCTION(BlueprintCallable, Category="Camera|BehaviorTree")
	UBlackboardComponent* GetCameraBlackboardComponent() const { return BBComponent; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BTComponent;

	// 为相机行为树维护的黑板组件
	UPROPERTY()
	TObjectPtr<UBlackboardComponent> BBComponent;
};
