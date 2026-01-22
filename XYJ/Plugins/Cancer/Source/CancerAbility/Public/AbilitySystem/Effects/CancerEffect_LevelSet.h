#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CancerEffect_LevelSet.generated.h"

/**
 * 等级
 */
UCLASS()
class CANCERABILITY_API UCancerEffect_LevelSet : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UCancerEffect_LevelSet();
};
