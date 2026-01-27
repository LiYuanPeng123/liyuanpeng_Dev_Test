#pragma once

#include "CoreMinimal.h"
#include "RootMotionModifier.h"
#include "RootMotionModifier_Simple3DWarp.generated.h"

/**
 * 3D 线性位移扭曲修饰器
 * 专门解决动画 Z 轴没有位移但需要向目标 Z 轴移动的问题
 */
UCLASS(meta = (DisplayName = "Simple 3D Warp"))
class CANCERABILITY_API URootMotionModifier_Simple3DWarp : public URootMotionModifier_Warp
{
	GENERATED_BODY()

public:
	URootMotionModifier_Simple3DWarp(const FObjectInitializer& ObjectInitializer);
	
	virtual void Update(const FMotionWarpingUpdateContext& Context) override;

	// 位移处理，Z 轴采用线性插值
	virtual FTransform ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	static URootMotionModifier_Simple3DWarp* AddRootMotionModifierSimple3DWarp(
		UMotionWarpingComponent* InMotionWarpingComp,
		const UAnimSequenceBase* InAnimation,
		float InStartTime,
		float InEndTime,
		FName InWarpTargetName,
		bool bInWarpTranslation,
		bool bInIgnoreZAxis,
		bool bInWarpRotation
	);

protected:
	// 标记当前是否存在有效目标（仅用于内部决策）
	bool bHasValidTargetCached = false;
};
