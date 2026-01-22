#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotifyState.h"
#include "AnimNotifyState_TranceActor.generated.h"

class UTargetingPreset;
/**
 * 
 */
UCLASS(DisplayName="物品检测窗口")
class CANCERABILITY_API UAnimNotifyState_TranceActor : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
};
