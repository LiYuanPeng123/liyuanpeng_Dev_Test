
#include "AbilitySystem/Tasks/AbilityTak_WaitTimeAndRest.h"
#include "AbilitySystemGlobals.h"

UAbilityTak_WaitTimeAndRest::UAbilityTak_WaitTimeAndRest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Time = 0.f;
	TimeStarted = 0.f;
}

void UAbilityTak_WaitTimeAndRest::Activate()
{
	ResetTimer(Time);
}

FString UAbilityTak_WaitTimeAndRest::GetDebugString() const
{
	if (UWorld* World = GetWorld())
	{
		const float TimeLeft = Time - World->TimeSince(TimeStarted);
		return FString::Printf(TEXT("WaitDelay. Time: %.2f. TimeLeft: %.2f"), Time, TimeLeft);
	}
	else
	{
		return FString::Printf(TEXT("WaitDelay. Time: %.2f. Time Started: %.2f"), Time, TimeStarted);
	}
}

UAbilityTak_WaitTimeAndRest* UAbilityTak_WaitTimeAndRest::WaitTimeAndRest(UGameplayAbility* OwningAbility, float Time)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(Time);

	UAbilityTak_WaitTimeAndRest* MyObj = NewAbilityTask<UAbilityTak_WaitTimeAndRest>(OwningAbility);
	MyObj->Time = Time;
	return MyObj;
}

void UAbilityTak_WaitTimeAndRest::ResetTimer(float NewTime)
{
	UWorld* World = GetWorld();
	TimeStarted = World->GetTimeSeconds();
	if (NewTime >= 0.f)
	{
		Time = NewTime;
	}
	if (TimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	if (Time <= 0.0f)
	{
		World->GetTimerManager().SetTimerForNextTick(this, &UAbilityTak_WaitTimeAndRest::OnTimeFinish);
	}
	else
	{
		// Use a dummy timer handle as we don't need to store it for later but we don't need to look for something to clear

		World->GetTimerManager().SetTimer(TimerHandle, this, &UAbilityTak_WaitTimeAndRest::OnTimeFinish, Time, false);
	}
}

void UAbilityTak_WaitTimeAndRest::OnTimeFinish()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnFinish.Broadcast();
	}
	EndTask();
}
