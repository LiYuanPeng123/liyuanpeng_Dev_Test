#pragma once

#include "CoreMinimal.h"
#include "RootMotionModifier.h"
#include "RootMotionModifier_DisplacementWarp.generated.h"

UCLASS(meta = (DisplayName = "Displacement Warp"))
class CANCERABILITY_API URootMotionModifier_DisplacementWarp : public URootMotionModifier_Warp
{
	GENERATED_BODY()

public:
	URootMotionModifier_DisplacementWarp(const FObjectInitializer& ObjectInitializer);

	// 是否保留原始位移节奏（按原动画每帧比例缩放）
	UPROPERTY(BlueprintReadWrite, Category = "Config", meta = (EditCondition = "bWarpTranslation"))
	bool bPreserveOriginalRhythm = true;

	// 无目标时的期望水平距离（2D），>0 生效
	UPROPERTY(BlueprintReadWrite, Category = "Config", meta = (EditCondition = "bWarpTranslation"),
		DisplayName="无目标位移距离")
	float NoTargetDistance = -1.f;

	// 有目标时的最大水平位移（2D）上限，>0 生效
	UPROPERTY(BlueprintReadWrite, Category = "Config", meta = (EditCondition = "bWarpTranslation"),
		DisplayName="最大位移距离(有目标)")
	float MaxDisplacementWithTarget = 300.f;

	// 触发“对目标进行修正”的检测距离（单位：厘米；<=0 表示始终修正）
	UPROPERTY(BlueprintReadWrite, Category = "Config", meta = (EditCondition = "bWarpTranslation"),
		DisplayName="触发修正检测距离(水平cm)")
	float CorrectionTriggerDistance = 800.f;

	// 直接距离覆盖（水平2D），>0 则覆盖上面所有距离计算
	UPROPERTY(BlueprintReadWrite, Category = "Config", meta = (EditCondition = "bWarpTranslation"),
		DisplayName="覆盖位移距离(2D)")
	float DirectDistanceOverride = 0.f;

	// 最终目标的世界空间额外偏移（用于微调）
	UPROPERTY(BlueprintReadWrite, Category = "Config")
	FVector FinalOffset = FVector::ZeroVector;

	// 覆盖以支持无目标情况且不置 Disabled
	virtual void Update(const FMotionWarpingUpdateContext& Context) override;

	// 位移处理，支持保留节奏与均匀位移两种策略
	virtual FTransform ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	static URootMotionModifier_DisplacementWarp* AddRootMotionModifierDisplacementWarp(
		UMotionWarpingComponent* InMotionWarpingComp,
		const UAnimSequenceBase* InAnimation,
		float InStartTime,
		float InEndTime,
		FName InWarpTargetName,
		EWarpPointAnimProvider InWarpPointAnimProvider,
		FTransform InWarpPointAnimTransform,
		FName InWarpPointAnimBoneName,
		bool bInWarpTranslation,
		bool bInIgnoreZAxis,
		bool bInWarpRotation,
		EMotionWarpRotationType InRotationType,
		EMotionWarpRotationMethod InRotationMethod,
		float InWarpRotationTimeMultiplier,
		float InWarpMaxRotationRate,
		bool bInPreserveOriginalRhythm,
		float InNoTargetDistance,
		float InMaxDisplacementWithTarget,
		float InDirectDistanceOverride,
		FVector InFinalOffset
	);

protected:
	// 标记当前是否存在有效目标（仅用于内部决策）
	bool bHasValidTargetCached = false;
};
