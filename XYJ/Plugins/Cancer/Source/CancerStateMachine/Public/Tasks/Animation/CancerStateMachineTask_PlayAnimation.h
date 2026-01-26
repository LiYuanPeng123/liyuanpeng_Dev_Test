#pragma once

#include "CoreMinimal.h"
#include "CancerStateMachineTask.h"
#include "Animation/AnimMontage.h"
#include "CancerStateMachineTask_PlayAnimation.generated.h"

/**
 * UCancerStateMachineTask_PlayMontage
 * 播放动画蒙太奇的任务
 */
UCLASS(DisplayName = "Play Montage")
class CANCERSTATEMACHINE_API UCancerStateMachineTask_PlayAnimation : public UCancerStateMachineTask
{
	GENERATED_BODY()

public:
	UCancerStateMachineTask_PlayAnimation(const FObjectInitializer& ObjectInitializer);

	/** 要播放的蒙太奇 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<UAnimMontage> MontageToPlay;

	/** 播放速率 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	float PlayRate;

	/** 开始播放位置 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	float StartingPosition;

	/** 开始播放的 Section */
	UPROPERTY(EditAnywhere, Category = "Animation")
	FName StartingSection;
	
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
};
