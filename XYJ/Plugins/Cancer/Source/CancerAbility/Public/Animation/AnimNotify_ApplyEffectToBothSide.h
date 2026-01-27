#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Animation/CancerCombatAnimNotify.h"
#include "AnimNotify_ApplyEffectToBothSide.generated.h"

/**
 * 向攻击方与受击方同时应用一组GE
 */
UCLASS()
class CANCERABILITY_API UAnimNotify_ApplyEffectToBothSide : public UCancerCombatAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(Blueprintreadwrite, EditAnywhere)
	TArray<TSubclassOf<UGameplayEffect>> ApplyToAttackerGE;

	UPROPERTY(Blueprintreadwrite, EditAnywhere)
	TArray<TSubclassOf<UGameplayEffect>> ApplyToHitTargetGE;
};
