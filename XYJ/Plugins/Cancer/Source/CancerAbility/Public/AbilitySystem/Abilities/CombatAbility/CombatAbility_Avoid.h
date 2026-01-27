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

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void SetAvoidLocRot(AActor* Attacker,AActor* Victim,FVector& Loc,FRotator& Rot);
protected:
	UPROPERTY(EditAnywhere,DisplayName="判断吸附距离")
	float LocDis = 100;
	UPROPERTY(EditAnywhere,DisplayName="吸附到目标的距离")
	FVector TargetLoc = FVector(100,0,50);
};
