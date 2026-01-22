#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "CancerCombatAnimationProvider.generated.h"


class UCancerGameplayAbility;

UCLASS(Const, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced,
	meta = (DisplayName = "Combat Animation Provider"))
class CANCERABILITY_API UCancerCombatAnimationProvider : public UObject
{
	GENERATED_BODY()

public:
	UCancerCombatAnimationProvider();

	UFUNCTION(BlueprintNativeEvent, Category = "Animation Provider")
	UAnimMontage* GetMontageToPlay(UCancerGameplayAbility* CombatAbility) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Animation Provider")
	FName GetSectionName(UCancerGameplayAbility* CombatAbility) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Animation Provider")
	float GetPlayRate(UCancerGameplayAbility* CombatAbility) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Animation Provider")
	float GetRootMotionScale(UCancerGameplayAbility* CombatAbility) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Provider")
	TObjectPtr<UAnimMontage> DefaultAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Provider")
	FGameplayTag MontageTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Provider")
	FName DefaultSectionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Provider")
	float DefaultPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Provider")
	float RootMotionScale;
};
