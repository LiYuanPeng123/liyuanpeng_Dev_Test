#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotify.h"
#include "AnimNotify_CameraShake.generated.h"

UCLASS(DisplayName="CameraShake")
class CANCERABILITY_API UAnimNotify_CameraShake : public UCancerCombatAnimNotify
{
public:
	GENERATED_BODY()
	UAnimNotify_CameraShake();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;

	UPROPERTY(EditAnywhere, Category = CameraShake)
	TSubclassOf<class UCameraShakeBase> CameraShake;
};
