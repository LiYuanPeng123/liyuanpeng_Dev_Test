
#pragma once

#include "CoreMinimal.h"
#include "CancerLockingPointComponent.generated.h"

/**
 * CancerLockingPointComponent
 * 
 * 锁定点组件，用于标记场景中可被锁定的位置
 * 可以附加到任何Actor上，使其成为可锁定目标
 */
UCLASS(ClassGroup=(Cancer), BlueprintType,Blueprintable,meta=(BlueprintSpawnableComponent))
class CANCERLOCKTARGET_API UCancerLockingPointComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsOpen = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxLockingDistance = 1000;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxUnLockingDistance = 1000;
protected:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface
};
