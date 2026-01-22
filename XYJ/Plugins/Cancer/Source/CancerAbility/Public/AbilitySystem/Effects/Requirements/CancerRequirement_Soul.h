#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectCustomApplicationRequirement.h"
#include "CancerRequirement_Soul.generated.h"

/**
 * 气力耗尽条件检测
 */
UCLASS()
class CANCERABILITY_API UCancerRequirement_Soul : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()


	UCancerRequirement_Soul(const FObjectInitializer& ObjectInitializer);

	// -- Begin Custom Application Requirement implementation
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
	                                                   const FGameplayEffectSpec& Spec,
	                                                   UAbilitySystemComponent* ASC) const override;
	// -- End Custom Application Requirement implementation

protected:
	FGameplayAttribute SoulAttribute;
};
