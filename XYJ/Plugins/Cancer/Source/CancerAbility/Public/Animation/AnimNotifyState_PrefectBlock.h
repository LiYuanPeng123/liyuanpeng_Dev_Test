// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotifyState.h"
#include "AnimNotifyState_PrefectBlock.generated.h"

/**
 * 弹反窗口
 */
UCLASS(DisplayName="弹反窗口")
class CANCERABILITY_API UAnimNotifyState_PrefectBlock : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()
public:
	UAnimNotifyState_PrefectBlock();
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
};
