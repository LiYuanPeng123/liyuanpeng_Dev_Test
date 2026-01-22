#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_MotionWarping.h"
#include "AnimNotifyState_HitDisplacementScale.generated.h"

/**
 * 击退位移缩放通知（根据缓存的击退距离缩放根运动位移）
 */
UCLASS(DisplayName="击退位移缩放")
class CANCERABILITY_API UAnimNotifyState_HitDisplacementScale : public UAnimNotifyState_MotionWarping
{
    GENERATED_BODY()

public:
    UAnimNotifyState_HitDisplacementScale(const FObjectInitializer& ObjectInitializer);

    // 当Z轴不参与距离计算时，仅缩放XY（Z保留为1）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HitWarp", DisplayName="忽略Z轴")
    bool bIgnoreZAxis = true;

    // 是否保留原始位移节奏（由URootMotionModifier_Scale处理）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HitWarp", DisplayName="保留原节奏")
    bool bPreserveOriginalRhythm = true;

    // 当无缓存击退距离时的默认距离（<=0则不生效）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HitWarp", DisplayName="默认击退距离")
    float DefaultDistance = -1.f;

    virtual URootMotionModifier* AddRootMotionModifier_Implementation(UMotionWarpingComponent* MotionWarpingComp,
        const UAnimSequenceBase* Animation, float StartTime, float EndTime) const override;
};