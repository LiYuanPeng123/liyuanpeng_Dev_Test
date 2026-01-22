#pragma once

#include "CoreMinimal.h"
#include "CancerGameplayEffect.h"
#include "CancerEffect_Upgrade.generated.h"





/**
 * 升级
 */
UCLASS()
class CANCERABILITY_API UCancerEffect_Upgrade : public UCancerGameplayEffect
{
	GENERATED_BODY()

public:
	UCancerEffect_Upgrade(const FObjectInitializer& ObjectInitializer);
	virtual void PostLoad() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	TObjectPtr<UCurveTable> CurveTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	FName CurveRowName = TEXT("MaxHealth");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	FName CurveRowName_MaxSoul = TEXT("MaxSoul");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	FName CurveRowName_MaxStamina = TEXT("MaxStamina");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	FName CurveRowName_MaxEnergy = TEXT("MaxEnergy");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	FName CurveRowName_AttackPower = TEXT("AttackPower");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	FName CurveRowName_PhysicalAttackPower = TEXT("PhysicalAttackPower");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	FName CurveRowName_Defense = TEXT("Defense");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	FName CurveRowName_DamageBonusPct = TEXT("DamageBonusPct");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	FName CurveRowName_DamageReductionPct = TEXT("DamageReductionPct");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	FName CurveRowName_Impact = TEXT("Impact");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	FName CurveRowName_IronBody = TEXT("IronBody");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cancer)
	float UpgradeCoeff = 0.f;

private:
	void BuildUpgradeModifiers();
};
