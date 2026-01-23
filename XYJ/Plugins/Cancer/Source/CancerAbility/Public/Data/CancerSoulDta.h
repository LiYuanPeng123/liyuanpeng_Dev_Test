#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameFramework/CancerDamageType.h"
#include "CancerSoulDta.generated.h"


USTRUCT(BlueprintType)
struct FCancerSoulData : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="虚弱硬直")
	FCancerStaggerInfo StaggerInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="回复气力的参数")
	TMap<ECancerHitType, FVector> ReplyValueMaps;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="能被震脱武器")
	bool bIsUnequipWeapon = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="伤害增幅倍率")
	float DamageCoefficient = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="虚弱时长")
	float WeakTime = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="虚弱结束回满气力的时间")
	float SoulReplyTime = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="回复气力时间")
	float ReplyTime = 2.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="格挡免疫", meta=(ClampMin = 0, ClampMax = 1))
	float SoulBonusPct = 0.5f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="弹反气力伤害系数", meta=(ClampMin = 0))
	float PerfectBlockDamageSoul = 2.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly ,Category="战斗参数配置",
		meta = (RequiredAssetDataTags = "RowStructure=/Script/CancerAbility.CombatImpactCosmeticsTableRow"))
	TObjectPtr<UDataTable> SurfaceImpactTable;


	FCancerSoulData()
	{
		ReplyValueMaps.Add(ECancerHitType::MeleeWeapon, FVector(0.5, 0.2, 10));
		ReplyValueMaps.Add(ECancerHitType::RemoteWeapon, FVector(0.5, 0.2, 10));
		ReplyValueMaps.Add(ECancerHitType::MagicArts, FVector(0.5, 0.2, 10));
		ReplyValueMaps.Add(ECancerHitType::StrikeBack, FVector(0.5, 0.2, 10));
		ReplyValueMaps.Add(ECancerHitType::Execute, FVector(0.5, 0.2, 10));
		ReplyValueMaps.Add(ECancerHitType::Level, FVector(0.5, 0.2, 10));
	}
};

/*
/**
 * 气力相关配置
 #1#
UCLASS()
class CANCERABILITY_API UCancerSoulDta : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
};
*/
