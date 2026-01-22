
#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotifyState.h"
#include "AnimNotifyState_PostPhase.generated.h"

/**
 * 
 */
UCLASS(DisplayName="后摇窗口(闪避等)")
class CANCERABILITY_API UAnimNotifyState_PostPhase : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()

public:
	// -- Begin Notify State implementation
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	// -- End Notify State implementation
	
	
};
