
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LatentActions.h"
#include "UObject/WeakObjectPtr.h"
#include "FWCountdownAction.generated.h"


UENUM(BlueprintType)
enum class ECoutDownInput:uint8
{
	Start UMETA(DisplayName = "Start"),

	Cancel UMETA(DisplayName = "Cancel"),
};

UENUM(BlueprintType)
enum class ECoutDownOutput:uint8
{
	Updated UMETA(DisplayName = "Updated"),

	Completed UMETA(DisplayName = "Completed"),

	Cancelled UMETA(DisplayName = "Cancelled"),
};


class FWCountdownAction : public FPendingLatentAction
{
	public:
	FWCountdownAction(float InTotalCountTime,float InUpdateInterval,float& InOutRemainingTime,
		ECoutDownOutput& InCountDownOutput,const FLatentActionInfo& InLatenInfo)
		: TotalCountTime(InTotalCountTime)
		, UpdateInterval(FMath::Max(0.f, InUpdateInterval))
		, OutRemainingTime(InOutRemainingTime)
		, CountDownOutput(InCountDownOutput)
		, LatentInfo(InLatenInfo)
		, ExecutionFunction(InLatenInfo.ExecutionFunction)
		, OutputLink(InLatenInfo.Linkage)
		, CallbackTarget(InLatenInfo.CallbackTarget)
	{
		OutRemainingTime = TotalCountTime;
		AccumulatedInterval = 0.f;
		bCancelled = false;
	}
	
	virtual void UpdateOperation(FLatentResponse& Response) override;
	
	void CancelAction();
protected:
	float TotalCountTime;
	float UpdateInterval;
	float& OutRemainingTime;
	float AccumulatedInterval;
	bool bCancelled;
	ECoutDownOutput& CountDownOutput;
	FLatentActionInfo LatentInfo;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	
};
