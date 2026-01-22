#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CancerEffect_IronBody.generated.h"

/**
 * 霸体效果 - 根据等级提供不同级别的霸体保护
 * 等级0 = 无霸体，等级1 = 1级霸体，以此类推
 */
UCLASS()
class CANCERABILITY_API UCancerEffect_IronBody : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UCancerEffect_IronBody();
};