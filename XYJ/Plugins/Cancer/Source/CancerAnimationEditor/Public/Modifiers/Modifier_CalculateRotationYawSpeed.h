// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "Modifier_CalculateRotationYawSpeed.generated.h"

/**
 * 
 */
UCLASS(DisplayName="RotationYawSpeed")
class CANCERANIMATIONEDITOR_API UModifier_CalculateRotationYawSpeed : public UAnimationModifier
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RotationYawSpeedCurveName = FName("RotationYawSpeed");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RootBoneName = FName("root");
	
	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;
};
