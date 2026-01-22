#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CancerEffect_EquipFromSource.generated.h"

/**
 * 装备绑定效果
 */
UCLASS()
class CANCERABILITY_API UCancerEffect_EquipFromSource : public UGameplayEffect
{
    GENERATED_BODY()
public:
    UCancerEffect_EquipFromSource();
};