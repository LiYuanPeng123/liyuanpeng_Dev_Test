// 

#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotifyState.h"
#include "Animations/CameraAnimationCameraModifier.h"
#include "AnimNotifyState_ExecuteCameraAnim.generated.h"

/**
 * 
 */
UCLASS(DisplayName="执行相机动画")
class CANCERABILITY_API UAnimNotifyState_ExecuteCameraAnim : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                         float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera,
		DisplayName="相机序列")
	UCameraAnimationSequence* CameraAnimationSequence;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera,
		DisplayName="相机参数")
	FCameraAnimationParams CameraParams;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera,
		DisplayName="停止时混出")
	bool bImmediate = false;
};
