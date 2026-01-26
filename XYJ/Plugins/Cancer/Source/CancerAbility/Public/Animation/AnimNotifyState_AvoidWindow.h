// 

#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotifyState.h"
#include "AnimNotifyState_AvoidWindow.generated.h"

/**
 * 
 */
UCLASS(DisplayName="跳踩窗口")
class CANCERABILITY_API UAnimNotifyState_AvoidWindow : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()
	public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
