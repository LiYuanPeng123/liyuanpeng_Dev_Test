
#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotifyState.h"
#include "AnimNotifyState_LinkHit.generated.h"

/**
 * 
 */
UCLASS(DisplayName="LinkHit")
class CANCERABILITY_API UAnimNotifyState_LinkHit : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HitID;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
};
