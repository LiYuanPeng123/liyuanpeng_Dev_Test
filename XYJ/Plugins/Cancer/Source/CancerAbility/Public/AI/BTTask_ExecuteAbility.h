#pragma once

#include "CoreMinimal.h"
#include "CancerAbilitySystemComponent.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ExecuteAbility.generated.h"

/**
 * 
 */
UCLASS(DisplayName="执行技能")
class CANCERABILITY_API UBTTask_ExecuteAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ExecuteAbility();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                            EBTNodeResult::Type TaskResult) override;

	void OnAbilityCancelled();

	void OnAbilityEnd(UGameplayAbility* GameplayAbility);

	
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTagContainer AbilityTag;
	FDelegateHandle AbilityEndedHandle;
	
	UPROPERTY(EditAnywhere, Category="Ability")
	bool bTreatCancelAsSuccess = true;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	TWeakObjectPtr<UCancerAbilitySystemComponent> CachedASC;
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	TWeakObjectPtr<UGameplayAbility> BoundAbility;
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	FDelegateHandle AbilityCancelledHandle;
};
