#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CancerEffect_EquipItemBonus.generated.h"

/**
 * 装备/武器加成：通过 SetByCaller 一次性把最终的装备/武器加成注入 CombatSet。
 * ——换装时先移除旧效果，再应用新效果。
 */
UCLASS()
class CANCERABILITY_API UCancerEffect_EquipItemBonus : public UGameplayEffect
{
    GENERATED_BODY()
public:
    UCancerEffect_EquipItemBonus();

    // SetByCaller 
    static const FName SetByCaller_AttackPowerBonus;
    static const FName SetByCaller_DefenseBonus;
    static const FName SetByCaller_DamageBonusPct;
    static const FName SetByCaller_DamageReductionPct;
};