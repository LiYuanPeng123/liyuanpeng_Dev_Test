#pragma once

#include "CoreMinimal.h"
#include "CancerSoulDta.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GameFramework/CancerDamageType.h"
#include "CancerCombatData.generated.h"

class UCancerWuXueData;

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="蒙太奇标签")
	FGameplayTag MontageTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="允许蓄力")
	bool bIsStrength = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="共用时长")
	float StrengthTime = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="判定时长")
	float DetermineDuration = 0.6f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="最大蓄力时间")
	float MaximumChargingTime = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="蓄力前摇动画的动画标签")
	FGameplayTag PreMontageTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="蓄力释放动画的动画标签")
	FGameplayTag PostMontageTag = FGameplayTag::EmptyTag;
};

USTRUCT(BlueprintType)
struct FWeaponWXData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="技能")
	TArray<FWeaponData> WeaponDatas;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="派生技能")
	TArray<FWeaponData> WeaponLinkDatas;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="闪避派生技能")
	FWeaponData DodgeLinkCombo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="弹反派生技能")
	FWeaponData PerfectBlockLinkCombo;
};


USTRUCT(BlueprintType)
struct FCancerComboWXData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TMap<FGameplayTag, FWeaponWXData> WeaponInfo;
};

USTRUCT(BlueprintType)
struct FCancerComboData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FGameplayTag ComboTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FWeaponData ComboData;
};


/*
 * 战斗参数配置表
 */
UCLASS()
class CANCERABILITY_API UCancerCombatData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCancerSoulData SoulData;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		meta = (RequiredAssetDataTags = "RowStructure=/Script/CancerAbility.CombatImpactCosmeticsTableRow"))
	TObjectPtr<UDataTable> SurfaceImpactTable;
};
