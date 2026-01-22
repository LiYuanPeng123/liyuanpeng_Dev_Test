#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AnimNotifyState_AddTags.generated.h"


/**
 * 
 */
UCLASS(DisplayName="添加标签")
class CANCERABILITY_API UAnimNotifyState_AddTags : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, DisplayName="添加标签")
	FGameplayTagContainer Tags;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;

	virtual void HandleTimeExpired(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual FString GetNotifyName_Implementation() const override;
};
