#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilityTak_WaitTimeAndRest.generated.h"

/**
 * 
 */
UCLASS()
class CANCERABILITY_API UAbilityTak_WaitTimeAndRest : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FWaitDelayDelegate OnFinish;

	virtual void Activate() override;

	/** Return debug string describing task */
	UFUNCTION(BlueprintCallable)
	virtual FString GetDebugString() const override;

	/** Wait specified time. This is functionally the same as a standard Delay node. */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTak_WaitTimeAndRest* WaitTimeAndRest(UGameplayAbility* OwningAbility, float Time);

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks")
	void ResetTimer(float NewTime);

private:
	void OnTimeFinish();

	float Time;
	float TimeStarted;
	FTimerHandle TimerHandle;
};
