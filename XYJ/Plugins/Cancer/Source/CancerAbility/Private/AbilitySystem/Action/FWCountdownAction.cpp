// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Action/FWCountdownAction.h"
#include "Engine/World.h"
#include "Engine/LatentActionManager.h"

void FWCountdownAction::UpdateOperation(FLatentResponse& Response)
{
	if (bCancelled)
	{
		OutRemainingTime = 0.f;
		CountDownOutput = ECoutDownOutput::Cancelled;
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		return;
	}

	const float DeltaSeconds = Response.ElapsedTime();
	OutRemainingTime = FMath::Clamp(OutRemainingTime - DeltaSeconds, 0.f, TotalCountTime);
	AccumulatedInterval += DeltaSeconds;

	if (OutRemainingTime <= 0.f)
	{
		CountDownOutput = ECoutDownOutput::Completed;
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		return;
	}

	if (UpdateInterval > 0.f && AccumulatedInterval >= UpdateInterval)
	{
		AccumulatedInterval = 0.f;
		CountDownOutput = ECoutDownOutput::Updated;
		Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);
	}
}

void FWCountdownAction::CancelAction()
{
	bCancelled = true;
}
