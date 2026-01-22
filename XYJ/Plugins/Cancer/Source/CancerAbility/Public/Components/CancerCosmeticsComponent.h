
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "CancerCosmeticsComponent.generated.h"


class UNiagaraSystem;
class UNiagaraComponent;

USTRUCT(BlueprintType)
struct FCombatImpactCosmetics
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	TObjectPtr<const AActor> Source = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	TObjectPtr<const APawn> Instigator = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	FHitResult HitResult = FHitResult();

	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	FGameplayTagContainer SourceTags = FGameplayTagContainer::EmptyContainer;

	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	FGameplayTagContainer TargetTags = FGameplayTagContainer::EmptyContainer;

	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	TSoftObjectPtr<UNiagaraSystem> ImpactParticles;

	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	TSoftObjectPtr<UNiagaraSystem> TrailParticles;
	
	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	TSoftObjectPtr<USoundBase> Sound;

	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	TSoftClassPtr<UCameraShakeBase> CameraShake;
	
	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
	
	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	TObjectPtr<UNiagaraComponent> ImpactTrailComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Cosmetics Parameter")
	TObjectPtr<UAudioComponent> AudioComponent;
	
};
/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = Cancer, meta = (BlueprintSpawnableComponent))
class CANCERABILITY_API UCancerCosmeticsComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	UCancerCosmeticsComponent();

	// -- Begin Actor Component implementation
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// -- End Actor Component implementation

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Ninja Combat|Components|Weapon Cosmetics")
	virtual void HandleImpactCosmetics(const FCombatImpactCosmetics& Parameters);

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = "Cosmetics")
	void HandleImpactParticles(UNiagaraSystem* Particles, const FCombatImpactCosmetics& Parameters) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = "Cosmetics")
	void HandleImpactTrailParticles(UNiagaraSystem* Particles, const FCombatImpactCosmetics& Parameters) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = "Cosmetics")
	void HandleImpactSound(USoundBase* Sound, const FCombatImpactCosmetics& Parameters) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = "Cosmetics")
	void HandleImpactCameraShake(TSubclassOf<UCameraShakeBase> CameraShakeClass, const FCombatImpactCosmetics& Parameters) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = "Cosmetics")
	void ModifyImpactParticleComponent(UNiagaraComponent* NiagaraComponent, const FCombatImpactCosmetics& Parameters) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = "Cosmetics")
	void ModifyImpactAudioComponent(UAudioComponent* AudioComponent, const FCombatImpactCosmetics& Parameters) const;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Ninja Combat|Components|Weapon Cosmetics")
	static void PlayCameraShakeOnController(APlayerController* PlayerController, const TSubclassOf<UCameraShakeBase>& CameraShakeClass, float Scale);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = "Cosmetics")
	float GetImpactCameraShakeScale(const FCombatImpactCosmetics& Parameters) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Cosmetics")
	FName NiagaraCallbackParameterName = TEXT("user.Data");

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Cosmetics")
	TObjectPtr<UNiagaraComponent> ImpactTrailComponent;
	

};
