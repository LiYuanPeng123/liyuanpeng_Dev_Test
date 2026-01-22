
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FCombatImpactCosmeticsTableRow.generated.h"

class UNiagaraSystem;
class USoundBase;
class UCameraShakeBase;



/**
 * 定义表面材质
 */
USTRUCT(BlueprintType)
struct CANCERABILITY_API FCombatImpactCosmeticsTableRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Particles played on hit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact Cosmetics")
	TSoftObjectPtr<UNiagaraSystem> Particles;

	/** Trail played on hit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact Cosmetics")
	TSoftObjectPtr<UNiagaraSystem> Trail;

	/** Sound asset used when on hit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact Cosmetics")
	TSoftObjectPtr<USoundBase> Sound;

	/** Camera shake used on impact. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact Cosmetics")
	TSoftClassPtr<UCameraShakeBase> CameraShake;
};
