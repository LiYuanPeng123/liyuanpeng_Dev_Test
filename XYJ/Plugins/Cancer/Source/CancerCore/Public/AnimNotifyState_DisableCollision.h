// 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_DisableCollision.generated.h"

/**
 * 
 */
UCLASS(DisplayName="禁用角色碰撞(CoreInterface)")
class CANCERCORE_API UAnimNotifyState_DisableCollision : public UAnimNotifyState
{
	GENERATED_BODY()

	public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;


	virtual FString GetNotifyName_Implementation() const override;
	
};
