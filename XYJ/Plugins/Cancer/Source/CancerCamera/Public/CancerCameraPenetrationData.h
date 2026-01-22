
#pragma once

#include "CoreMinimal.h"
#include "CancerPenetrationAvoidanceFeeler.h"
#include "Engine/DataAsset.h"
#include "CancerCameraPenetrationData.generated.h"

/**
 * 摄像机碰撞参数设置
 */
UCLASS()
class CANCERCAMERA_API UCancerCameraPenetrationData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UCancerCameraPenetrationData();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FCancerPenetrationAvoidanceFeeler> PenetrationAvoidanceFeelers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CollisionPushOutDistance = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PenetrationBlendInTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PenetrationBlendOutTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ReportPenetrationPercent = 0.f;
};