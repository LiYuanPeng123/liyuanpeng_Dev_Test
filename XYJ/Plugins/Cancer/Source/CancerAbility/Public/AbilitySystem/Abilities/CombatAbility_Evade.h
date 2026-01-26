#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CombatAbility_Evade.generated.h"

/**
 * 闪避
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_Evade : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()

public:
	UCombatAbility_Evade(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	virtual void OnAnimationReady_Implementation() override;

	virtual void OnAnimationFinished_Implementation() override;

	virtual void OnAnimationStarted_Implementation() override;

	virtual void AbilityInputStarted_Implementation() override;

	UPROPERTY(BlueprintReadOnly)
	int32 ExecuteIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxExecuteIndex = 3;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName OrientDefaultSection = FName("B");

	UPROPERTY()
	FVector2D AerialPitchOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evade")
	float MaxAerialPitchOffset = 45.0f;

protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName WarpName;
	UPROPERTY()
	FVector StartAccel2D;


private:
};
