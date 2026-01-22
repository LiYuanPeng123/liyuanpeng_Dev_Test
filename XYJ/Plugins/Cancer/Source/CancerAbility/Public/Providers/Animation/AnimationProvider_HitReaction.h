#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Providers/CancerCombatDirectionalAnimationProvider.h"
#include "AnimationProvider_HitReaction.generated.h"


USTRUCT(BlueprintType)
struct FHitReactionContext
{
	GENERATED_BODY()

	/** Context for this Hit Reaction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Hit Context")
	TObjectPtr<UAnimMontage> AnimMontage;
};


UCLASS(meta = (DisplayName = "Directional Hits"))
class CANCERABILITY_API UAnimationProvider_HitReaction : public UCancerCombatDirectionalAnimationProvider
{
	GENERATED_BODY()

public:
	// -- Begin Animation Provider implementation
	virtual UAnimMontage* GetMontageToPlay_Implementation(UCancerGameplayAbility* CombatAbility) const override;
	// -- End Animation Provider implementation

	// -- Begin Direction Animation Provider implementation
	virtual bool CalculateAnimationAngle_Implementation(const UCancerGameplayAbility* CombatAbility,
	                                                    float& OutAngle) const override;
	// -- End Direction Animation Provider implementation
};
