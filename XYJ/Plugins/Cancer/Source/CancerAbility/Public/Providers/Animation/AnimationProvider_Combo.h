#pragma once

#include "CoreMinimal.h"
#include "Providers/CancerCombatAnimationProvider.h"
#include "AnimationProvider_Combo.generated.h"

USTRUCT(BlueprintType)
struct FCombatAnimMontage
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Context")
	TObjectPtr<UAnimMontage> AnimMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Context")
	FGameplayTag AnimMontageTag;
};

/**
 * 
 */
UCLASS()
class CANCERABILITY_API UAnimationProvider_Combo : public UCancerCombatAnimationProvider
{
	GENERATED_BODY()

public:
	// -- Begin Animation Provider implementation
	virtual UAnimMontage* GetMontageToPlay_Implementation(UCancerGameplayAbility* CombatAbility) const override;
	// -- End Animation Provider implementation

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Context")
	bool bUseTag = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Context")
	TArray<FCombatAnimMontage> CombatAnimMontages;
};
