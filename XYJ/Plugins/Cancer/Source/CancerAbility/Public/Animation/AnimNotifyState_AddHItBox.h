
#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotifyState.h"
#include "AnimNotifyState_AddHItBox.generated.h"

/**
 * 添加碰撞盒体
 */
UCLASS()
class CANCERABILITY_API UAnimNotifyState_AddHItBox : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	virtual void HandleTimeExpired(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	UPROPERTY(EditAnywhere,Category=HitBox)
	TEnumAsByte<ECollisionChannel> Channel;
	UPROPERTY(EditAnywhere,Category=HitBox)
	FVector HitBox;
	UPROPERTY(EditAnywhere,Category=HitBox)
	FVector HitBoxOffset;
	UPROPERTY(EditAnywhere,Category=HitBox)
	FName AttachName;

protected:

	UPROPERTY(Transient)
	TObjectPtr<class UBoxComponent> SpawnedBox = nullptr;

	
};
