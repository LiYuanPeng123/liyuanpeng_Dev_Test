
#pragma once

#include "CoreMinimal.h"
#include "Providers/CancerCombatAnimationProvider.h"
#include "AnimationProvider_Random.generated.h"


USTRUCT(BlueprintType)
struct FRandomAnimationContext
{
	GENERATED_BODY()

	/** Animation Montage used as an option. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Animation Context")
	TObjectPtr<UAnimMontage> AnimMontage;
	
};

UCLASS(meta = (DisplayName = "Random"))
class CANCERABILITY_API UAnimationProvider_Random : public UCancerCombatAnimationProvider
{
	GENERATED_BODY()

public:

	// -- Begin Animation Provider implementation
	virtual UAnimMontage* GetMontageToPlay_Implementation(UCancerGameplayAbility* CombatAbility) const override;
	// -- End Animation Provider implementation

protected:
	
	/** List of random Animation Montages that can be provided. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Provider", meta = (TitleProperty = "AnimMontage"))
	TArray<FRandomAnimationContext> RandomAnimations;
	
};
