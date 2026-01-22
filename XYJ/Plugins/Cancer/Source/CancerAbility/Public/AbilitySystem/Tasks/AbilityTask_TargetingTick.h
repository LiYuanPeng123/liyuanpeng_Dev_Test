#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Types/TargetingSystemTypes.h"
#include "AbilityTask_TargetingTick.generated.h"

class UTargetingPreset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetingTickEvent, FTargetingRequestHandle, TargetData);

UCLASS()
class CANCERABILITY_API UAbilityTask_TargetingTick : public UAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FTargetingTickEvent OnTargetUpdated;

	UAbilityTask_TargetingTick();

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UAbilityTask_TargetingTick* TargetingPresetTick(UGameplayAbility* OwningAbility, UTargetingPreset* InPreset);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

protected:
	UPROPERTY()
	TObjectPtr<UTargetingPreset> TargetingPreset;

	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;
};
