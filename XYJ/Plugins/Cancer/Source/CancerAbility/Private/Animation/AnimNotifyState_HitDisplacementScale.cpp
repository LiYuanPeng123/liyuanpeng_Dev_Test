#include "Animation/AnimNotifyState_HitDisplacementScale.h"

#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"
#include "Components/CancerMotionWarpingComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotifyState_HitDisplacementScale)


UE_DISABLE_OPTIMIZATION
UAnimNotifyState_HitDisplacementScale::UAnimNotifyState_HitDisplacementScale(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootMotionModifier = ObjectInitializer.CreateDefaultSubobject<URootMotionModifier_Scale>(
		this, TEXT("RootMotionModifier_Scale"));
}

URootMotionModifier* UAnimNotifyState_HitDisplacementScale::AddRootMotionModifier_Implementation(
	UMotionWarpingComponent* MotionWarpingComp, const UAnimSequenceBase* Animation, float StartTime,
	float EndTime) const
{
	if (!MotionWarpingComp || !Animation)
	{
		return Super::AddRootMotionModifier_Implementation(MotionWarpingComp, Animation, StartTime, EndTime);
	}

	// 从扩展的 CancerMotionWarpingComponent 中消费待缩放距离（击退距离）
	float DesiredDistance = -1.f;
	if (UCancerMotionWarpingComponent* CancerComp = Cast<UCancerMotionWarpingComponent>(MotionWarpingComp))
	{
		DesiredDistance = CancerComp->ConsumePendingScaleDistance();
	}
	if (DesiredDistance <= 0.f)
	{
		DesiredDistance = DefaultDistance;
	}

	if (DesiredDistance > 0.f)
	{
		// 计算本通知窗口内的原始根运动位移长度
		const FTransform TotalRM = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(
			Animation, StartTime, EndTime);
		const FVector TotalT = TotalRM.GetTranslation();
		const float OriginalDistance = bIgnoreZAxis ? TotalT.Size2D() : TotalT.Size();

		if (OriginalDistance > KINDA_SMALL_NUMBER)
		{
			const float ScaleFactor = DesiredDistance / OriginalDistance;
			const FVector ScaleVec = bIgnoreZAxis ? FVector(ScaleFactor, ScaleFactor, 1.f) : FVector(ScaleFactor);

			// 保留原始节奏（由Scale修饰器按比例缩放位移）
			if (URootMotionModifier* Mod = URootMotionModifier_Scale::AddRootMotionModifierScale(
				MotionWarpingComp, Animation, StartTime, EndTime, ScaleVec))
			{
				return Mod;
			}
		}
	}

	return Super::AddRootMotionModifier_Implementation(MotionWarpingComp, Animation, StartTime, EndTime);
}
UE_ENABLE_OPTIMIZATION