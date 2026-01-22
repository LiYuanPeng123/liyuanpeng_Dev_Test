// 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ChargedAttackCost.generated.h"

class UCancerAbilityCost;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChargedAttackCostFailed);

/**
 * 
 */
UCLASS()
class CANCERABILITY_API UAbilityTask_ChargedAttackCost : public UAbilityTask
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_ChargedAttackCost* ChargedAttackCost(UGameplayAbility* OwningAbility,
	                                                         TArray<UCancerAbilityCost*> Costs);

	UPROPERTY(BlueprintAssignable)
	FChargedAttackCostFailed OnCostFailed;

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

protected:
	UPROPERTY()
	TArray<TObjectPtr<UCancerAbilityCost>> Costs;
};
