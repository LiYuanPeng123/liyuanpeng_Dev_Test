
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectCustomApplicationRequirement.h"
#include "CancerRequirement_Dead.generated.h"

/**
 * 死亡检测条件
 */
UCLASS()
class CANCERABILITY_API UCancerRequirement_Dead : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()

	
	UCancerRequirement_Dead(const FObjectInitializer& ObjectInitializer);
	
	// -- Begin Custom Application Requirement implementation
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* ASC) const override;
	// -- End Custom Application Requirement implementation

protected:

	FGameplayAttribute HealthAttribute;
};
