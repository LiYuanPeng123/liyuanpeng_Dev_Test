#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "Modifier_CreateCurves.generated.h"

USTRUCT(BlueprintType)
struct FAnimationCurveKey
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer", Meta = (ClampMin = 0))
	int32 Frame{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer")
	float Value{0.0f};
};

USTRUCT(BlueprintType)
struct FAnimationCurve
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer")
	uint8 bAddKeyOnEachFrame : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer")
	TArray<FAnimationCurveKey> Keys
	{
		{0, 0.0f}
	};
};

UCLASS()
class CANCERANIMATIONEDITOR_API UModifier_CreateCurves : public UAnimationModifier
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bOverrideExistingCurves : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<FAnimationCurve> Curves{
		FAnimationCurve(FName("Layering_Legs")),
		FAnimationCurve(FName("Layering_Pelvis")),
		FAnimationCurve(FName("Layering_Spine")),
		FAnimationCurve(FName("Layering_Head")),
		FAnimationCurve(FName("Layering_Arm_L")),
		FAnimationCurve(FName("Layering_Arm_R")),
		FAnimationCurve(FName("Layering_Hand_R")),
		FAnimationCurve(FName("Layering_Hand_L")),
		FAnimationCurve(FName("Layering_Hand_R")),
		FAnimationCurve(FName("Layering_Arm_L_LS")),
		FAnimationCurve(FName("Layering_Arm_R_LS")),
	};

public:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;
};
