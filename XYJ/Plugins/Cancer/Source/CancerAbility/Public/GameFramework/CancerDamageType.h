#pragma once

#include "CoreMinimal.h"
#include "CancerMeleeScan.h"
#include "CancerNativeGamePlayTag.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "Types/EDamageDetectionSource.h"
#include "UObject/Object.h"
#include "CancerDamageType.generated.h"


enum class EMeleeScanMode : uint8;

UENUM(BlueprintType)
enum class ECancerHitType:uint8
{
	MeleeWeapon UMETA(DisplayName ="近战武器"),
	RemoteWeapon UMETA(DisplayName ="远程武器"),
	MagicArts UMETA(DisplayName ="法术"),
	StrikeBack UMETA(DisplayName ="反击"),
	Execute UMETA(DisplayName ="处决"),
	Level UMETA(DisplayName ="关卡"),
};

UENUM(BlueprintType)
enum class ECancerType:uint8
{
	Attacker UMETA(DisplayName ="Attacker"),
	Victim UMETA(DisplayName ="Victim"),
	Both UMETA(DisplayName ="Both"),
};

UENUM(BlueprintType)
enum class ECancerFeedbackType:uint8
{
	Invulnerable UMETA(DisplayName ="免疫"),
	PerfectBlock UMETA(DisplayName ="弹反"),
	Block UMETA(DisplayName ="格挡"),
	DamageOnly UMETA(DisplayName ="仅伤害"),
};


USTRUCT(BlueprintType)
struct FCancerStaggerInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="冲击力", meta = (ClampMin = "0"))
	int32 ImpactForce = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="硬直动画ID")
	int32 StaggerAnimationID = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="硬直时长", meta = (ClampMin = "0.0"))
	float HardDuration = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="击退距离", meta = (ClampMin = "0.0"))
	float KnockbackDistance = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="命中触发目标技能Tag")
	FGameplayTag TriggerTargetAbilityTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="命中触发目标技能时长")
	float TriggerTargetAbilityDuration = 0.0f;
};


USTRUCT(BlueprintType)
struct FCancerHitInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, DisplayName="伤害来源")
	AActor* SourceActor = nullptr;
	UPROPERTY(BlueprintReadWrite, DisplayName="伤害施加者")
	AActor* CapActor = nullptr;
	UPROPERTY(BlueprintReadWrite, DisplayName="最终具体伤害来源")
	AActor* FinalDamageActor = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="伤害类型|")
	ECancerHitType HitType = ECancerHitType::MeleeWeapon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="气力伤害系数")
	float SoulValue = 1.f;
	UPROPERTY(BlueprintReadWrite)
	FHitResult HitResult = FHitResult();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="命中伤害事件标签")
	FGameplayTag DamageTag{Tag_Combat_Event_AbilityTrigger_ReceiveHit};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="GC标签")
	FGameplayTag DamageGCTag = Tag_GameplayCue_Damage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="GC旋转")
	FRotator GCRotation = FRotator(0, 0, 0);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="伤害系数")
	float Damage = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="蓄力伤害增幅曲线")
	FRuntimeFloatCurve ChargeupDamage = FRuntimeFloatCurve();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="冲击力", meta = (ClampMin = "0"))
	int32 ImpactForce = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="硬直动画ID")
	int32 StaggerAnimationID = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="硬直时长", meta = (ClampMin = "0.0"))
	float HardDuration = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="击退距离", meta = (ClampMin = "0.0"))
	float KnockbackDistance = 0.0f;
	
	// 允许子弹指定受击者要触发的 Ability Tag
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="命中触发目标技能Tag")
	FGameplayTag TriggerTargetAbilityTag;
	
	//  控制时长，将通过 EventMagnitude 传递给目标技能
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="命中触发目标技能时长")
	float TriggerTargetAbilityDuration = 0.0f;
};


USTRUCT(BlueprintType)
struct FCancerTranceInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Scan", DisplayName="触发检测源")
	EDamageDetectionSource CombatAbilitySource = EDamageDetectionSource::Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Scan", DisplayName="检测模式")
	EMeleeScanMode ScanMode = EMeleeScanMode::BoxSweep;
	UPROPERTY(EditAnywhere, Category = "Melee Scan", DisplayName="检测武器插槽",
		meta=(EditCondition = "CombatAbilitySource == EDamageDetectionSource::Weapon ", EditConditionHides))
	TArray<FName> WeaponNames;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Scan", DisplayName="检测插槽")
	TArray<FName> SocketNames;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Scan", DisplayName="插槽偏移")
	TMap<FName, FVector> TraceSocketOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Scan", DisplayName="检测目标")
	TArray<TEnumAsByte<ECollisionChannel>> ScanChannels;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Scan", NoClear, DisplayName="伤害检测类")
	TSubclassOf<UCancerMeleeScan> MeleeScanClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Scan",
		meta = (EditCondition = "ScanMode == EMeleeScanMode::BoxSweep", EditConditionHides)
		, DisplayName="盒体检测配置")
	FVector BoxHalfExtent = FVector(30.f, 30.f, 30.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Scan",
		meta = (EditCondition = "ScanMode == EMeleeScanMode::CapsuleSweep", EditConditionHides)
		, DisplayName="胶囊体检测配置")
	FVector CapsuleExtent = FVector(30.f, 30.f, 30.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Scan",
		meta = (EditCondition = "ScanMode == EMeleeScanMode::SphereSweep", EditConditionHides)
		, DisplayName="球体检测配置")
	float SphereRadius = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Scan", DisplayName="自定义检测半径")
	float TraceRadius = 30.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="允许多次检测")
	bool bAllowMultipleHits = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="检测次数（/s）")
	int32 Hits = 10;

	FCancerTranceInfo()
	{
		ScanMode = EMeleeScanMode::LineTrace;
		ScanChannels.Add(ECollisionChannel::ECC_Pawn);
	}
};

USTRUCT(BlueprintType)
struct CANCERABILITY_API FCancerComboInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "命中|配置", DisplayName="镜头抖动")
	TSubclassOf<UCameraShakeBase> CameraShake = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "命中|配置", DisplayName="类型")
	ECancerType Type = ECancerType::Attacker;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="命中|配置", DisplayName="曲线")
	FRuntimeFloatCurve SolmoCurve = FRuntimeFloatCurve();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="命中|配置", DisplayName="时长")
	float SolmoTime = 0.f;
};


USTRUCT(BlueprintType)
struct FCancerHitEffectInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "类型", DisplayName="可格挡")
	bool bBlocked = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "类型", meta=(EditCondition ="bBlocked"), DisplayName="格挡GC标签")
	FGameplayTag GCBlockType = Tag_GameplayCue_Block;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "类型", meta=(EditCondition ="bBlocked"), DisplayName="格挡GC旋转")
	FRotator GCBlockRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "类型", DisplayName="可反击")
	bool bReflected = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "类型", meta=(EditCondition ="bReflected"),
		DisplayName="弹反GC标签")
	FGameplayTag GCPerfectBlockType = Tag_GameplayCue_PerfectBlock;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "类型", meta=(EditCondition ="bReflected"),
		DisplayName="弹反GC旋转")
	FRotator GCPerfectBlockRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "类型", DisplayName="可被无敌帧躲避")
	bool bInvincible = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "类型", meta=(EditCondition ="bInvincible"),
		DisplayName="闪避GC标签")
	FGameplayTag GCInvulnerableType = Tag_GameplayCue_Invulnerable;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "类型", meta=(EditCondition ="bInvincible"),
		DisplayName="闪避GC旋转")
	FRotator GCInvulnerableRotation = FRotator::ZeroRotator;
};

// 伤害命中后的反馈信息
USTRUCT(BlueprintType)
struct FCancerDamageFeedback
{
	GENERATED_BODY()

	// 额外信息 例如 击杀 气绝
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "命中|反馈", DisplayName="额外信息")
	FGameplayTagContainer DamageInfo;
};


USTRUCT(BlueprintType)
struct FCancerDamageParameter
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "伤害检测", DisplayName="伤害检测配置")
	FCancerTranceInfo TranceInfo;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "伤害", DisplayName="伤害配置")
	FCancerHitInfo HitInfo;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "命中", DisplayName="命中配置")
	FCancerComboInfo ComboInfo;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "类型", DisplayName="类型配置")
	FCancerHitEffectInfo HitEffectInfo;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "反馈", DisplayName="伤害反馈")
	FCancerDamageFeedback DamageFeedback;
};


/**
 *  伤害类型
 */
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType)
class CANCERABILITY_API UCancerDamageType : public UObject
{
	GENERATED_BODY()

public:
	UCancerDamageType();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="禁用伤害检测")
	bool bEnable = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Scan", DisplayName="启用调试")
	bool bEnableDebug = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="DamageParameter")
	FCancerDamageParameter DamageParameter;

	UFUNCTION(blueprintCallable, BlueprintPure, Category = "Melee Scan")
	const AActor* GetSourceActor() const;
	UFUNCTION(blueprintCallable, Category = "Melee Scan")
	void SetSourceActor(AActor* InSourceActor);

	UFUNCTION(blueprintCallable, BlueprintPure, Category = "Melee Scan")
	ECancerHitType GetHitType() const;

	UFUNCTION(blueprintCallable, Category = "Melee Scan")
	const FCancerHitInfo& GetHitInfo() const;

	UFUNCTION(blueprintCallable, Category = "Melee Scan")
	const FCancerComboInfo& GetComboInfo();

	UFUNCTION(blueprintCallable, Category = "Melee Scan")
	const FCancerTranceInfo& GetTraceInfo() const;
	UFUNCTION(blueprintCallable, Category = "Melee Scan")
	void SetTraceInfo(const FCancerTranceInfo& InTraceInfo);

	UFUNCTION(blueprintCallable, BlueprintPure, Category = "Melee Scan")
	void SetHitResult(const FHitResult& InHit);

	UFUNCTION(blueprintCallable, Category = "Melee Scan")
	const FCancerHitEffectInfo& GetHitEffectInfo() const;

	UFUNCTION(blueprintCallable, BlueprintPure, Category = "Melee Scan")
	void SetHitEffectInfo(const FCancerHitEffectInfo& InHitEffect);

	UFUNCTION(blueprintCallable, BlueprintPure, Category = "Melee Scan")
	FHitResult& GetHitResult();

	UFUNCTION(blueprintCallable, Category = "Melee Scan")
	void SetCancerHitInfo(const FCancerHitInfo& HitInfo);
};
