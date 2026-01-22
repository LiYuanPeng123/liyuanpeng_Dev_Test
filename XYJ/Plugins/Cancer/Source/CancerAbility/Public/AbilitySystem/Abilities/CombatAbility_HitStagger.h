#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "Data/StaggerAnimationData.h"
#include "CombatAbility_HitStagger.generated.h"


USTRUCT()
struct CANCERABILITY_API FCancerHit
{
	GENERATED_BODY()
	UPROPERTY()
	UAnimMontage* Montage = nullptr;
	UPROPERTY()
	FName PrimarySection = NAME_None;
	UPROPERTY()
	float MontageDuration = 0.0f;
	UPROPERTY()
	float KnockbackDistance = 0.0f;
};
/**
 * 受击硬值
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_HitStagger : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()

public:
	UCombatAbility_HitStagger(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

protected:
	
	bool CanCheckHit( const FGameplayEventData* TriggerEventData,FCancerHit& OutHit);

	void ApplyPerfectBlockOverride(const class UCancerDamageType* DamageType, float& ImpactForce, int32& StaggerAnimationID, FCancerHit& OutHit);

	FName GetDirectionalSectionName(const FVector& HitDirection, EStaggerAnimationDirection DirectionType);
	
	UFUNCTION(BlueprintCallable)
	int32 GetIronBodyLevel();

	UFUNCTION(BlueprintCallable)
	bool CanBeHit(float ImpactForce);

	UFUNCTION(BlueprintCallable)
	FVector CalculateHitDirection(const FVector& AttackerLocation, const FVector& HitLocation) const;
	
	void ScaleHitDuration(UAnimMontage* Montage, float TargetDuration,FName SelectSection);
	
	float GetSectionDuration(UAnimMontage* Montage, const FName& SectionName) const;

	void SetSectionPlayRate(UAnimMontage* Montage, const FName& SectionName, float PlayRate);

	UFUNCTION()
	FStaggerAnimationConfig GetHitAnimData(EStaggerType Module, int32 AnimationID,
	bool bExactMatch = true);

	UFUNCTION()
	EStaggerType GetStaggerType();

	private:
	bool bLinkHit;
	
};
