
#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotifyState.h"
#include "AnimNotifyState_RooTurn.generated.h"

/**
 *  根运动期间 开启输入旋转
 */
UCLASS(DisplayName="Root Turn")
class CANCERABILITY_API UAnimNotifyState_RooTurn : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()
	public:

	
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;


	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

	protected:
	UPROPERTY(EditAnywhere,DisplayName="旋转速率",BlueprintReadOnly)
	float RotationYaw = 360.f;
};
