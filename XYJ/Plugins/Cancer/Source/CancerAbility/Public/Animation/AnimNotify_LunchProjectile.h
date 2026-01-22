#pragma once

#include "CoreMinimal.h"
#include "Animation/CancerCombatAnimNotify.h"
#include "GameFramework/CancerDamageType.h"
#include "GameFramework/CancerProjectileActor.h"
#include "AnimNotify_LunchProjectile.generated.h"


class ACancerProjectileActor;


USTRUCT(BlueprintType)
struct FSpawnProjectileType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, DisplayName="禁用预览")
	bool bEnable{false};
	UPROPERTY(EditAnywhere, DisplayName="禁用此配置")
	bool bDisableActor{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="发射物类",
		meta=(MustImplement = "/Script/CancerAbility.ICombatProjectileInterface"))
	TSubclassOf<ACancerProjectileActor> ProjectileActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="发射物标签")
	TMap<FName, FString> ProjectileTagMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Projectile", DisplayName="伤害")
	TArray<TObjectPtr<UCancerDamageType>> Damages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", DisplayName="发射物数据")
	TSoftObjectPtr<UCancerProjectileData> ProjectileData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", DisplayName="覆盖贝塞尔曲线偏移")
	bool bOverrideBezierOffset = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile",
		meta=(EditCondition = "bOverrideBezierOffset", EditConditionHides))
	FVector BezierOffset = FVector::Zero();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", DisplayName="覆盖运动参数")
	bool bOverrideMovementParams = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="初始速度",
		meta=(EditCondition = "bOverrideMovementParams", EditConditionHides))
	float InitSpeed{1000.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="最大速度",
		meta=(EditCondition = "bOverrideMovementParams", EditConditionHides))
	float MaxSpeed{1000.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="摩擦力系数",
		meta=(EditCondition = "bOverrideMovementParams", EditConditionHides))
	float Friction{0.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="重力系数",
		meta=(EditCondition = "bOverrideMovementParams", EditConditionHides))
	float Gravity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", DisplayName="覆盖模型效果")
	bool bOverrideMeshEffect = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="  模型的旋转",
		meta=(EditCondition = "bOverrideMeshEffect", EditConditionHides))
	FRotator MeshRotator{FRotator::ZeroRotator};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="  模型的大小缩放",
		meta=(EditCondition = "bOverrideMeshEffect", EditConditionHides))
	FVector MeshScale{FVector::Zero()};


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", DisplayName="发射物插槽")
	FName ProjectileSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", DisplayName="插槽位置偏移")
	FVector SocketLocationOffset = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", DisplayName="插槽旋转偏移")
	FRotator SocketRotationOffset = FRotator(0.0f, 0.0f, 0.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", DisplayName="使用角色朝向")
	bool UseOwnerRotation {true};
};

/**
 * 发射物
 */
UCLASS(DisplayName="发射物")
class CANCERABILITY_API UAnimNotify_LunchProjectile : public UCancerCombatAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_LunchProjectile();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;


	UFUNCTION(BlueprintNativeEvent, Category = "Projectile")
	UCancerMeleeScan* CreateMeleeScanInstance(AActor* Owner, AActor* Causer, UCancerDamageType* DamageType,
	                                          UMeshComponent* SourceMesh, float Duration) const;

	virtual FString GetNotifyName_Implementation() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Projectile)
	TArray<FSpawnProjectileType> SpawnProjectiles;

	UPROPERTY()
	TArray<UCancerMeleeScan*> MeleeScans;

	UPROPERTY()
	TArray<AActor*> Spawners;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", DisplayName="预览目标偏移")
	FVector TargetOffset = FVector(0.0f, 0.0f, 0.0f);
#if WITH_EDITORONLY_DATA
	// 预览窗口中扫描的调用间隔与持续时间
	UPROPERTY(EditAnywhere, Category = "EditorPreview", meta=(ClampMin="0.01"))
	float PreviewScanInterval = 0.1f;

	UPROPERTY(EditAnywhere, Category = "EditorPreview", meta=(ClampMin="0.01"))
	float PreviewScanDuration = 1.0f;

	UPROPERTY()
	class UStaticMeshComponent* PreviewSphere = nullptr;
#endif

#if WITH_EDITOR

private:
	// 预览扫描的状态与定时器
	FTimerHandle PreviewScanTimerHandle;
	bool bPreviewTimerStarted = false;
	float PreviewElapsed = 0.0f;
	TWeakObjectPtr<USkeletalMeshComponent> PreviewMeshComp;


	void StartPreviewScanTimer(USkeletalMeshComponent* MeshComp);
	void StopPreviewScanTimer();
	void PreviewScanTick();
#endif
};
