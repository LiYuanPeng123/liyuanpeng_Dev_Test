#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_CancelWindow.generated.h"

/**
 * 动画通知状态：取消窗口
 * - Begin：打开取消窗口（允许闪避/格挡等）
 * - End：关闭取消窗口
 */
UCLASS(DisplayName="Cancel Window")
class CANCERABILITY_API UAnimNotifyState_CancelWindow : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
