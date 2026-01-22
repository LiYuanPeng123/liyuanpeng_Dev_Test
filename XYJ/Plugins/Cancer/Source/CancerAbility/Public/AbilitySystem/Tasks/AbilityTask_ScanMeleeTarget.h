
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ScanMeleeTarget.generated.h"

class UCancerMeleeScan;
/**
 * 
 */
UCLASS()
class CANCERABILITY_API UAbilityTask_ScanMeleeTarget : public UAbilityTask
{
    GENERATED_BODY()
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWaitMeleeScan, const UCancerMeleeScan*, MeleeScan, const FGameplayAbilityTargetDataHandle&, Data);
public:

	/** Broadcasts targets that we hit during the scan. */
	UPROPERTY(BlueprintAssignable)
	FWaitMeleeScan OnMeleeTargetsFound;

	UAbilityTask_ScanMeleeTarget();

	// -- Begin Ability Task implementation
	virtual void TickTask(float DeltaTime) override;
    // -- End Ability Task implementation

	/**
	 * Creates the appropriate Ability Task, based on the provided data.
	 */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Ability|Tasks")
	static UAbilityTask_ScanMeleeTarget* CreateTask(UGameplayAbility* OwningAbility);

	/**
	 * Adds a new Melee Scan to this task.
	 * 
	 * @param MeleeScan
	 *		New Scan details to be joined in this task.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ninja Combat|Ability Tasks")
	void Join(const UCancerMeleeScan* MeleeScan);
	
protected:
    
    /** The start location used for the target broadcast. */
    FGameplayAbilityTargetingLocationInfo StartLocation;
    
    /** Current list of targets that were found. */
    TArray<FHitResult> HitResults;

    /** All instances participating in this scan. */
    UPROPERTY()
    TArray<TObjectPtr<UCancerMeleeScan>> MeleeScans;

    /** Per-scan, per-actor cooldowns to enforce Hits-per-second when multi-hit is enabled.
     *  Note: nested containers are not supported by UHT as UPROPERTY values, so keep it non-UPROPERTY. */
    TMap<UCancerMeleeScan*, TMap<TWeakObjectPtr<AActor>, float>> ActorCooldowns;
};
