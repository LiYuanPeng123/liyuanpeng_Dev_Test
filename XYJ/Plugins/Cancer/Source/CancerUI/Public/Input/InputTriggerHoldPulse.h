
/*
#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "InputTriggerHoldPulse.generated.h"
UENUM()
enum class ETriggerHoldPulseState : uint8
{
	//按住
	Hold,
	//脉冲
	Pulse,
};

UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "HoldPulse"))
class  UInputTriggerHoldPulse : public UInputTriggerTimedBase
{
	GENERATED_BODY()
	
protected:
	virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput,
													 FInputActionValue ModifiedValue, float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, DisplayName = "脉冲启动时间", meta = (ClampMin = "0"))
	float PulseStartTime = 0.5f;
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, DisplayName = "脉冲间隔时间", meta = (ClampMin = "0"))
	float PulseIntervalTime = 0.06f;

private:
	ETriggerHoldPulseState TriggerHoldPulseState;
	int32 TriggerCount = 0;
};
*/
