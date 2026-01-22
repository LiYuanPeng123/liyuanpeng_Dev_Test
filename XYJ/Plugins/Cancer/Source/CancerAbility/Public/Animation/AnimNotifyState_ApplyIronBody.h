
#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "CancerCombatAnimNotifyState.h"
#include "AnimNotifyState_ApplyIronBody.generated.h"

/**
 * 
 */
UCLASS(DisplayName="应用霸体")
class CANCERABILITY_API UAnimNotifyState_ApplyIronBody : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()

	public:
	UAnimNotifyState_ApplyIronBody();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IronBodyLevel;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
};
