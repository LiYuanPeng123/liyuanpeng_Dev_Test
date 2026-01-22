// GShenLin

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/CancerCombatAnimNotifyState.h"
#include "AnimNotifyState_Priority.generated.h"

/**
 * 技能优先级状态管理
 */
UCLASS()
class CANCERABILITY_API UAnimNotifyState_Priority : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer AbilityTag;

	UPROPERTY(EditAnywhere)
	int32 Priority;
	
	virtual FString GetNotifyName_Implementation() const override;
};
