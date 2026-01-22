
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CancerEffect_Soul.generated.h"

class UAdditionalEffectsGameplayEffectComponent;
/**
 *  持续回复气力
 */
UCLASS()
class CANCERABILITY_API UCancerEffect_Soul : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UCancerEffect_Soul(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
