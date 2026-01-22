#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComboInputConfig.generated.h"

UENUM(BlueprintType)
enum class EComboDirectionReference : uint8
{
	Actor,
	Camera
};

UCLASS(BlueprintType)
class CANCERABILITY_API UComboInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BufferWindowSeconds = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DoubleTapMaxGapSeconds = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HoldThresholdSeconds = 0.30f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ChordWindowSeconds = 0.10f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AxisDeadzone = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AxisCardinalThreshold = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EComboDirectionReference DirectionReference = EComboDirectionReference::Actor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxBufferEvents = 16;
};
