
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CancerCombatAnimNotifyState.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS(Abstract)
class CANCERABILITY_API UCancerCombatAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UCancerCombatAnimNotifyState();
	// -- Begin Notify State implementation
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	// -- End Notify State implementation
	/**
	 * Provides the ASC related to the Mesh's owner.
	 */
	static UAbilitySystemComponent* GetAbilitySystemComponent(const AActor* Owner);
	
protected:

	bool bExpired {false};
	float PlayRate;
	float ElapsedTime;
	float NotifyDuration;
	float EffectiveDuration;
	
	virtual float GetActualPlayRate(AActor* Owner, const UAnimSequenceBase* Animation) const;	

	virtual void HandleTimeExpired(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};
