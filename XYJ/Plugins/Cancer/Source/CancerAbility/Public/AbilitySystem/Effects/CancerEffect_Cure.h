
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "UObject/Object.h"
#include "CancerEffect_Cure.generated.h"

/**
 * 治疗
 */
UCLASS()
class CANCERABILITY_API UCancerEffect_Cure : public UGameplayEffect
{
	GENERATED_BODY()
	public:
	UCancerEffect_Cure();
};
