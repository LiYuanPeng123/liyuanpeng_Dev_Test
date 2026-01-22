#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "CancerExecution_Soul.generated.h"

/**
 * 气力回复计算
 */
UCLASS()
class CANCERABILITY_API UCancerExecution_Soul : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UCancerExecution_Soul();


	// -- Begin Execution Calculation implementation
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	// -- End Execution Calculation implementation
};
