#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_PreInput.generated.h"

/**
 * 动画通知状态：预输入窗口
 * - Begin：打开预输入窗口（允许提前输入并暂存）
 * - End：关闭预输入窗口（清空暂存）
 */
UCLASS(DisplayName="Pre-Input Window")
class CANCERABILITY_API UAnimNotifyState_PreInput : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
