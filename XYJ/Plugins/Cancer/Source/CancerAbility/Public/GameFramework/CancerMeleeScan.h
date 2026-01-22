#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CancerMeleeScan.generated.h"


class UCancerDamageType;

UENUM(BlueprintType)
enum class EMeleeScanMode : uint8
{
	LineTrace UMETA(DisplayName = "线条检测"),
	BoxSweep UMETA(DisplayName = "盒体检测"),
	CapsuleSweep UMETA(DisplayName = "胶囊体检测"),
	SphereSweep UMETA(DisplayName = "球形检测"),
	BoxWithSelf,
	SphereWithSelf
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class CANCERABILITY_API UCancerMeleeScan : public UObject
{
	GENERATED_BODY()

public:
	bool bEnableDebug;
	/*
	 */
	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Melee Scan", DisplayName = "New Melee Scan Instance")
	static UCancerMeleeScan* NewInstance(TSubclassOf<UCancerMeleeScan> MeleeScanClass, AActor* Owner,
	                                     AActor* Instigator, UMeshComponent* ScanMesh, const TArray<FName>& SocketNames,
	                                     const TMap<FName, FVector>& TraceSocketOffset,
	                                     TArray<TEnumAsByte<ECollisionChannel>> ScanChannels,
	                                     EMeleeScanMode ScanMode,
	                                     FVector ShapeDimensions = FVector::ZeroVector, float TraceRadius = 30.0f,
	                                     float Duration = 1.5f);

	UFUNCTION(BlueprintPure, Category = "Cancer Combat|Melee Scan")
	AActor* GetInstigator() const { return Instigator; }

	//持续时间
	UFUNCTION(BlueprintPure, Category = "Cancer Combat|Melee Scan")
	float GetDuration() const { return Duration; }

	UFUNCTION(BlueprintNativeEvent, Category = "Melee Scan")
	void ScanForTargets(TArray<FHitResult>& OutHits);
	UFUNCTION(BlueprintPure, Category = "Melee Scan")
	virtual bool HasValidScanData() const;

	UPROPERTY(BlueprintReadOnly, Category = "Melee Scan")
	UCancerDamageType* DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Scan")
	bool bAllowMultipleHitsOnSameTarget;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Melee Scan")
	float Duration;

	UPROPERTY(BlueprintReadOnly, Category = "Melee Scan")
	TObjectPtr<AActor> ScanOwner;

	UPROPERTY(BlueprintReadOnly, Category = "Melee Scan")
	TObjectPtr<APawn> Instigator;

	UPROPERTY(BlueprintReadOnly, Category = "Melee Scan")
	FVector ShapeDimensions;

	UPROPERTY(BlueprintReadOnly, Category = "Melee Scan")
	float TraceRadius;

	//简单还是复杂碰撞
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combo Graph|Traces")
	bool bTraceComplex = true;

	UPROPERTY(BlueprintReadOnly, Category = "Melee Scan")
	TObjectPtr<UMeshComponent> ScanMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Melee Scan")
	TArray<FName> SocketNames;

	UPROPERTY(BlueprintReadOnly, Category = "Melee Scan")
	TMap<FName, FVector> TraceSocketOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Melee Scan")
	TArray<TEnumAsByte<ECollisionChannel>> ScanChannels;

	UPROPERTY(BlueprintReadOnly, Category = "Melee Scan")
	EMeleeScanMode ScanMode;
	/** 用于扫描的形状，如果扫描模式被设置为采用扫掠方式，则使用此形状。*/
	FCollisionShape ScanShape;

	TArray<FHitResult> ConsolidatedHits;

	// 自适应子步进配置：在低帧率或大幅位移/旋转情况下提高命中准确度
	UPROPERTY(EditDefaultsOnly, Category = "Melee Scan|Substepping")
	bool bAdaptiveSubstepping = true;

	// 单次 sweep 允许的最大位移长度（厘米），超过则拆分为多段
	UPROPERTY(EditDefaultsOnly, Category = "Melee Scan|Substepping", meta=(ClampMin=1.0, ClampMax=1000.0))
	float MaxSweepStepDistance = 50.0f;

	// 单次 sweep 允许的最大旋转角度（度），超过则拆分为多段
	UPROPERTY(EditDefaultsOnly, Category = "Melee Scan|Substepping", meta=(ClampMin=1.0, ClampMax=180.0))
	float MaxSweepStepAngleDeg = 30.0f;

	// 拆分的最大段数上限，避免过多调用造成性能问题
	UPROPERTY(EditDefaultsOnly, Category = "Melee Scan|Substepping", meta=(ClampMin=1, ClampMax=16))
	int32 MaxSubsteps = 6;

	UFUNCTION(BlueprintNativeEvent, Category = "Melee Scan")
	TArray<AActor*> GetIgnoredActors() const;

	/** 从当前网格中获取所有插槽位置的快照。*/
	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Melee Scan")
	virtual void SnapshotSocketPositions();

	/** 插槽的位置. */
	TMap<FName, FVector> LastSocketPositions;

	/** 插槽的上一帧旋转（世界空间），用于子步进旋转插值。*/
	TMap<FName, FQuat> LastSocketRotations;


	/**
	 * Creates the object parameters used for the scan query.
	 */
	virtual FCollisionObjectQueryParams CreateObjectParams() const;

	/**
	 * Creates the trace parameter used for the scan query.
	 */
	virtual FCollisionQueryParams CreateTraceParams() const;

private:
	void DrawScanLine(const FVector& StartPosition, const FVector& EndPosition) const;
	void DrawScanSweep(const FVector& StartPosition, const FQuat& Rotation) const;
	void DrawNewTarget(const FHitResult& NewHit) const;
	FVector GetAdjustedSocketPosition(const FName& SocketName) const;

	/**
	 * 根据距离与旋转角度拆分 sweep 为多段，提升低帧下命中率。
	 */
	void PerformSweepWithSubsteps(
		const UWorld* World,
		const FVector& Start,
		const FVector& End,
		const FQuat& StartRot,
		const FQuat& EndRot,
		const FCollisionObjectQueryParams& ObjectParams,
		const FCollisionQueryParams& TraceParams,
		TArray<FHitResult>& OutHits
	) const;
};
