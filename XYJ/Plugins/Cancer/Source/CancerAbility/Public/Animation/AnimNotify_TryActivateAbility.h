
#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_TryActivateAbility.generated.h"

/**
 * 
 */
UCLASS(DisplayName="激活技能(ASC)")
class CANCERABILITY_API UAnimNotify_TryActivateAbility : public UCancerCombatAnimNotify
{
	GENERATED_BODY()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Tag)
	FGameplayTag AbilityTag;
};
