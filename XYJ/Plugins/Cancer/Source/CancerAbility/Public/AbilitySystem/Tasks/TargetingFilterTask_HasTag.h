// GShenLin

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Tasks/TargetingFilterTask_BasicFilterTemplate.h"
#include "TargetingFilterTask_HasTag.generated.h"

/**
 * 过滤拥有特定Tag的目标
 */
UCLASS(Blueprintable)
class CANCERABILITY_API UTargetingFilterTask_HasTag : public UTargetingFilterTask_BasicFilterTemplate
{
	GENERATED_BODY()
	
public:
	
	/** Evaluation function called by derived classes to process the targeting request */
	virtual bool ShouldFilterTarget(const FTargetingRequestHandle& TargetingHandle, const FTargetingDefaultResultData& TargetData) const override;
	
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer TagContainer;
};
