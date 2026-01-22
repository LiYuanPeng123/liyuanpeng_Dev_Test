#pragma once

#include "CoreMinimal.h"
#include "MotionWarpingComponent.h"
#include "CancerMotionWarpingComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Cancer), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class CANCERABILITY_API UCancerMotionWarpingComponent : public UMotionWarpingComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, DisplayName="位移到目标Actor")
	void UpdateLocationAndRotation_Actor(const AActor* Target, float MaxDis, FVector Offset, FName WarpName,
	                                     bool bCancelWarpIfCloser = false);

	// bCancelWarpIfCloser: 为真时，若当前角色到目标的距离小于“Warp位置到目标”的距离，则取消本次 Warp
	UFUNCTION(BlueprintCallable, DisplayName="位移到目标Character(会减去胶囊体半径)")
	void UpdateLocationAndRotation_Character(const AActor* Target, float MaxDis, FVector Offset, FName WarpName,
	                                         bool bCancelWarpIfCloser = false);

	UFUNCTION(BlueprintCallable, DisplayName="从自身位移到(仅Yaw方向)")
	void UpdateLocationAndRotation_Self(FVector Offset, FName WarpName);

	UFUNCTION(BlueprintCallable, DisplayName="从自身位移到(指定距离，Yaw方向)")
	void UpdateLocationAndRotation_SelfDis(float Distance, FVector Offset, FName WarpName);

	// 设置/消费：根运动位移缩放的目标距离（例如击退距离）
	UFUNCTION(BlueprintCallable, DisplayName="设置待缩放距离")
	void SetPendingScaleDistance(float Distance);

	UFUNCTION(BlueprintCallable, DisplayName="消费待缩放距离")
	float ConsumePendingScaleDistance();

private:
	// 若>0表示存在待缩放距离，消费后重置为-1
	float PendingScaleDistance = -1.f;
};
