// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CombatAbility_Avoid.generated.h"

/**
 * 踩踏
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_Avoid : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FVector AvoidLocation;
	UPROPERTY(BlueprintReadWrite)
	FRotator AvoidRotation;
	UPROPERTY(editAnywhere)
	FName AvoidWarpName;

	virtual void OnAnimationReady_Implementation() override;
protected:
};
