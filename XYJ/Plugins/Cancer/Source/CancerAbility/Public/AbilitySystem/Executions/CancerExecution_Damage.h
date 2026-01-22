#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameFramework/CancerDamageType.h"
#include "CancerExecution_Damage.generated.h"

USTRUCT(BlueprintType,Blueprintable)
struct FCancerDamageExecutionParameters
{
	GENERATED_BODY()

	// Captured Attributes
	UPROPERTY(BlueprintReadWrite, Category="Attributes",DisplayName="攻击力")
	float AttackPower = 0.f;
	UPROPERTY(BlueprintReadWrite, Category="Attributes",DisplayName="气力攻击力")
	float PhysicalAttackPower = 0.f;
	UPROPERTY(BlueprintReadWrite, Category="Attributes",DisplayName="伤害加成")
	float DamageBonusPct = 0.f;
	UPROPERTY(BlueprintReadWrite, Category="Attributes",DisplayName="防御")
	float Defense = 0.f;
	UPROPERTY(BlueprintReadWrite, Category="Attributes",DisplayName="伤害减免")
	float DamageReductionPct = 0.f;

	// SetByCaller Magnitudes
	UPROPERTY(BlueprintReadWrite, Category="SetByCaller",DisplayName="伤害系数")
	float DamageCoeff = 0.f;
	UPROPERTY(BlueprintReadWrite, Category="SetByCaller",DisplayName="气力伤害系数")
	float SoulDamageCoeff = 0.f;
	UPROPERTY(BlueprintReadWrite, Category="SetByCaller",DisplayName="气力恢复系数X")
	float DamageCoeff_X = 0.f;
	UPROPERTY(BlueprintReadWrite, Category="SetByCaller",DisplayName="气力恢复系数Y")
	float DamageCoeff_Y = 0.f;
	UPROPERTY(BlueprintReadWrite, Category="SetByCaller",DisplayName="气力恢复系数Z")
	float DamageCoeff_Z = 0.f;

	// Context
	UPROPERTY(BlueprintReadWrite, Category="Context")
	AActor* SourceActor = nullptr;
	UPROPERTY(BlueprintReadWrite, Category="Context")
	AActor* TargetActor = nullptr;
	UPROPERTY(BlueprintReadWrite, Category="Context")
	const UCancerDamageType* DamageType = nullptr;
	
	// Tags
	UPROPERTY(BlueprintReadWrite, Category="Tags")
	FGameplayTagContainer SourceTags;
	UPROPERTY(BlueprintReadWrite, Category="Tags")
	FGameplayTagContainer TargetTags;
};

USTRUCT(BlueprintType)
struct FCancerDamageExecutionOutput
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Output",DisplayName="伤害")
	float FinalDamage = 0.f;

	UPROPERTY(BlueprintReadWrite, Category="Output",DisplayName="气力伤害")
	float FinalSoulDamage = 0.f; 

	UPROPERTY(BlueprintReadWrite, Category="Output",DisplayName="气力恢复")
	float RecoveredSoul = 0.f; 
};

/**
 * 伤害计算
 */
UCLASS(Blueprintable, BlueprintType)
class CANCERABILITY_API UCancerExecution_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:

	UCancerExecution_Damage();

	// -- Begin Execution Calculation implementation
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	// -- End Execution Calculation implementation
	
	void CalculateDamage(const FCancerDamageExecutionParameters& Params,
	                        FCancerDamageExecutionOutput& OutOutput) const;
	UFUNCTION(BlueprintNativeEvent, Category = "Calculation")
	void K2_CalculateBaseDamage(const FCancerDamageExecutionParameters& Params, float& OutDamage) const;
	UFUNCTION(BlueprintNativeEvent, Category = "Calculation")
	void K2_CalculateSoulDamage(const FCancerDamageExecutionParameters& Params, float& OutSoulDamage) const;
	UFUNCTION(BlueprintNativeEvent, Category = "Calculation")
	void K2_CalculateSoulRecovery(const FCancerDamageExecutionParameters& Params,const float RawDamage,const float PhysicalDamage, float& OutRecoveredSoul) const;
};
