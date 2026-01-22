
#include "AbilitySystem/Tasks/AbilityTask_ChargedAttackCost.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "AbilitySystem/Cost/CancerAbilityCost.h"


UAbilityTask_ChargedAttackCost* UAbilityTask_ChargedAttackCost::ChargedAttackCost(UGameplayAbility* OwningAbility,
	TArray<UCancerAbilityCost*> Costs)
{
	UAbilityTask_ChargedAttackCost* MyObj = NewAbilityTask<UAbilityTask_ChargedAttackCost>(OwningAbility);
	MyObj->Ability = OwningAbility;
	MyObj->Costs = Costs;
	return MyObj;
}

void UAbilityTask_ChargedAttackCost::Activate()
{
	bTickingTask = true;
}

void UAbilityTask_ChargedAttackCost::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	UCancerGameplayAbility* GA = Cast<UCancerGameplayAbility>(Ability);
	if (!GA)
		return;
	for (const TObjectPtr<UCancerAbilityCost>& AdditionalCost : Costs)
	{
		if (AdditionalCost != nullptr)
		{
			if (AdditionalCost->ShouldOnlyApplyCostOnHit())
			{
				continue;
			}
			if (!AdditionalCost->CheckCost(GA, GA->GetCurrentAbilitySpecHandle(), GA->GetCurrentActorInfo(), {}))
			{
				OnCostFailed.Broadcast();
				EndTask();
				return;
			}
			else
			{
				AdditionalCost->ApplyCost(GA, GA->GetCurrentAbilitySpecHandle(), GA->GetCurrentActorInfo(), {});
			}
		}
	}
}
