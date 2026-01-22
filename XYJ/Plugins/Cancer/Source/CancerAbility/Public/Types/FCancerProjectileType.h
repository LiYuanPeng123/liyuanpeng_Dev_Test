#pragma once
#include "CoreMinimal.h"
#include "FCancerProjectileType.generated.h"


class UTargetingPreset;
class UNiagaraSystem;
//发射物的瞄准方式
UENUM(BlueprintType)
enum class EProjectileAimingMode : uint8
{
	Forward UMETA(DisplayName = "Forward"),
	Targeting UMETA(DisplayName = "Targeting"),
	Location UMETA(DisplayName = "Location"),
};

//发射物运动模式
UENUM(BlueprintType)
enum class EProjectileVelocityMode : uint8
{
	Normal,
	BezierCurve
};

USTRUCT(BlueprintType)
struct FCancerProjectileType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="运动模式")
	EProjectileVelocityMode VelocityMode{EProjectileVelocityMode::Normal};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="贝塞尔在xyz上的偏移",
		meta=(EditCondition = "VelocityMode == EProjectileVelocityMode::BezierCurve", EditConditionHides))
	FVector BezierOffset{FVector::Zero()};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="贝塞尔达到拐点后的新速度",
		meta=(EditCondition = "VelocityMode == EProjectileVelocityMode::BezierCurve", EditConditionHides))
	float BezierFinishSpeed{2000.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="达到拐点后再次锁定目标",
		meta=(EditCondition = "VelocityMode == EProjectileVelocityMode::BezierCurve", EditConditionHides))
	bool bLockAgain{false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="瞄准模式")
	EProjectileAimingMode AimingMode{EProjectileAimingMode::Forward};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile",
		meta = (EditCondition = "AimingMode == EProjectileAimingMode::Targeting", EditConditionHides))
	bool bExecuteAsync = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile",
		meta = (EditCondition = "AimingMode == EProjectileAimingMode::Targeting", EditConditionHides))
	TObjectPtr<UTargetingPreset> TargetingPreset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="初始速度")
	float InitSpeed{1000.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="最大速度")
	float MaxSpeed{1000.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="摩擦力系数")
	float Friction{0.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="重力系数")
	float Gravity{1.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="插值移动")
	bool bInterpMovement{true};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="插值旋转")
	bool bInterpRotation{true};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName=" 最大生命周期")
	float LifeTime{3.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName=" 最远飞行距离")
	float MaxMoveDis{3000.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName=" 最大命中次数")
	int32 MaxHits{1};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", DisplayName="反弹次数")
	int32 MaxBounces{1};
};

USTRUCT(BlueprintType)
struct FCancerProjectileEffect
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="  模型的旋转")
	FRotator MeshRotator{FRotator::ZeroRotator};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="  模型的大小缩放")
	FVector MeshScale{FVector::Zero()};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="   模型是否残留在命中actor上")
	bool bStay{false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="   子弹的弹道特效")
	TSoftObjectPtr<UNiagaraSystem> TrailFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="   子弹的命中特效")
	TSoftObjectPtr<UNiagaraSystem> ImpactFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="   子弹的命中贴花材质")
	TSoftObjectPtr<UMaterialInterface> ImpactDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="   贴花的随机缩放范围大小")
	float ImpactDecalScaleBase{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="   贴花的随机缩放范围")
	FVector2D ImpactDecalRandomScaleRange{FVector2D(0.8f, 1.2f)};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="   子弹的弹道音效")
	TSoftObjectPtr<USoundBase> TrailSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="  子弹的命中音效")
	TSoftObjectPtr<USoundBase> ImpactSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="  子弹的对撞销毁特效")
	TSoftObjectPtr<UNiagaraSystem> DestructionFX;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Projectile", DisplayName="   子弹的对撞销毁音效")
	TSoftObjectPtr<USoundBase> DestructionSound;
};
