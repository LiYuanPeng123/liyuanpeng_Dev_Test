#pragma once

#include "CoreMinimal.h"
#include "CancerPoolableActor.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Interfaces/CombatProjectileInterface.h"
#include "Types/FCancerProjectileType.h"
#include "UObject/Object.h"
#include "Types/TargetingSystemTypes.h"
#include "CancerProjectileActor.generated.h"

class UCancerProjectileData;
class UGameplayEffect;
class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraSystem;
class USoundBase;
class UMaterialInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWaitProjectileScan, const UCancerMeleeScan*, MeleeScan, const FGameplayAbilityTargetDataHandle&, Data);
/**
 *  发射物
 */
UCLASS()
class CANCERABILITY_API ACancerProjectileActor : public ACancerPoolableActor, public ICombatProjectileInterface
{
	GENERATED_BODY()

public:


	UPROPERTY(BlueprintAssignable)
	FWaitProjectileScan OnProjectileScan;
	UFUNCTION(BlueprintNativeEvent, Category = "Attack")
	void HandleScanTargetsReceived(const UCancerMeleeScan* MeleeScan,
										const FGameplayAbilityTargetDataHandle& Data);
	
	ACancerProjectileActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// -- Begin Projectile implementation
	virtual TArray<AActor*> GetActorsHit_Implementation() const override;
	virtual float GetSpeed_Implementation() const override;
	virtual void PrepareLaunch_Implementation() override;
	virtual void Launch_Implementation() override;
	UFUNCTION(BlueprintPure, Category = "Combat Projectile")
	virtual FVector GetTargetLocation_Implementation() override{return TargetLocation;};
	UFUNCTION(BlueprintCallable, Category = "Combat Projectile")
	virtual void SetTargetLocation_Implementation(FVector InTargetLocation) override;
	virtual void InitProjectileData_Implementation(const UCancerProjectileData* InProjectileData) override;
	virtual int32 RegisterMeleeScanRequest_Implementation(UCancerMeleeScan* MeleeScan) override;
	virtual void CancelMeleeScanRequest_Implementation(int32 RequestId) override;
	virtual void Activate_Implementation() override;
	// -- End Projectile implementation


	UFUNCTION(BlueprintPure, Category = "Combat Projectile")
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	
	UFUNCTION(BlueprintPure, Category = "Combat Projectile")
	FORCEINLINE int32 GetBounceCount() const { return BounceCount; }
	
	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	void LaunchForward();
	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	void LaunchToTarget(const AActor* Target);
	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	void HandleTargetNotFound();
	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	void LaunchToLocation(FVector Location);

	

protected:

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UCancerProjectileData* ProjectileDataPtr;

	UPROPERTY(BlueprintReadOnly)
	EProjectileAimingMode AimingMode{EProjectileAimingMode::Forward};

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTargetingPreset> TargetingPreset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Projectile")
	bool bEnableDebug = true;
	
	UPROPERTY()
	TMap<int32, TObjectPtr<UCancerMeleeScan>> MeleeScanMap;

	TMap<UCancerMeleeScan*, TMap<TWeakObjectPtr<AActor>, float>> ActorCooldowns;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	FRotator GetInitialRotation() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	void SetupIgnoredActors();

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	void AdjustHitResult(const FHitResult& ImpactResult, FHitResult& AdjustedResult);

	// 安全运行时缓存，避免在 InitProjectileData 无效时解引用数据崩溃
	UPROPERTY(BlueprintReadOnly, Category = "Combat Projectile")
	bool bIsPrepared{false};

	UPROPERTY(BlueprintReadOnly, Category = "Combat Projectile")
	int32 CachedMaxBounces{0};

	UPROPERTY(BlueprintReadOnly, Category = "Combat Projectile")
	bool bTargetingExecuteAsync{false};


	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	void HandleActorHit(const FHitResult& ImpactResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	void HandleProjectileExhausted();

	virtual void ApplyGameplayEffectToData(const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION(BlueprintNativeEvent, Category = "Projectile")
	FGameplayTagContainer GetDynamicGameplayTags() const;


	UFUNCTION(BlueprintNativeEvent, Category = "Projectile")
	TMap<FGameplayTag, float> GetAdditionalSetByCallerMagnitudes() const;


	virtual void HandleTargetReceived(const FTargetingRequestHandle& TargetingHandle);

	static void PlayNiagaraSystem(const UWorld* World, UNiagaraSystem* System,
	                              const FVector& Location, const FRotator& Rotation);

	static void PlaySound(const UWorld* World, USoundBase* Sound, const FVector& Location,
	                      const FRotator& Rotation);

	virtual void OnActivation_Implementation() override;
	virtual void OnDeactivation_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	TSoftObjectPtr<UNiagaraSystem> SelectHitImpactParticles(const FHitResult& HitResult,
	                                                        const FGameplayTagContainer& SourceTags,
	                                                        const FGameplayTagContainer& TargetTags) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	TSoftObjectPtr<USoundBase> SelectHitImpactSound(const FHitResult& HitResult,
	                                                const FGameplayTagContainer& SourceTags,
	                                                const FGameplayTagContainer& TargetTags) const;


	UFUNCTION(BlueprintNativeEvent, Category = "Combat Projectile")
	TSoftClassPtr<UCameraShakeBase> SelectHitImpactCameraShake(const FHitResult& HitResult,
	                                                           const FGameplayTagContainer& SourceTags,
	                                                           const FGameplayTagContainer& TargetTags) const;

private:
    /** Checks if this projectile has been exhausted. */
    bool bHasExhausted;

	/** Target we are chasing after. */
	int32 TargetIndex;

    /** Current amount of bounces that happened. */
    int32 BounceCount;

	UPROPERTY()
	FVector LaunchVelocity;

	/** Velocity from last bounce, if any. */
	FVector LastBounceVelocity;

	/** Initial Location set, based on aiming strategy. */
	FVector TargetLocation;

	/** Handle to be used when the projectile hits. */
	FGameplayEffectSpecHandle ImpactEffectHandle;

    /** Movement Component for projectiles. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess))
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;


	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;

    /** List of all actors that were hit by this projectile. */
    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> ActorsHit;

    float CachedMaxMoveDistance{0.f};
    int32 CachedMaxHits{0};
    float AccumulatedTravelDistance{0.f};
    FVector LastDistanceSampleLocation{FVector::ZeroVector};

    bool bLockAgainAfterInflection{false};
    bool bHasRelockedAfterInflection{false};

    bool bHasBezierOffsetOverride{false};
    FVector BezierOffsetOverride{FVector::ZeroVector};

public:
    void SetBezierOffsetOverride(const FVector& InOffset, bool bEnable);

	/** Requests a sync/async targeting. */
	void ExecuteTargeting();

	/** Hidden handler to ensure internal logic is correct. */
	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

	/** Hidden handler to ensure internal logic is correct. */
	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	// -- Start Debug Helpers
	void PrintMessage(const FString& Message) const;
	void DrawScanLine(const FHitResult& NewHit) const;
	void DrawNewTarget(const FHitResult& NewHit, const FColor& Color) const;
	// -- End Debug Helpers

	bool bUseBezier{false};
	float BezierT{0.f};
	float BezierTotalLength{0.f};
	FVector BezierStart;
	FVector BezierControl;
	FVector BezierEnd;

	FVector BezierPoint(float T) const;
	float ComputeBezierLength() const;
};
