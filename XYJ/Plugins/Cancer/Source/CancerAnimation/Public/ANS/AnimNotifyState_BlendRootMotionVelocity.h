
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "AnimNotifyState_BlendRootMotionVelocity.generated.h"

/**
 * 混合根运动速度
 */
UCLASS(Abstract)
class CANCERANIMATION_API UAnimNotifyState_BlendRootMotionVelocity : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,DisplayName="混合比例")
	float Blend;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,DisplayName="根运动缩放")
	float Scale;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	
};
