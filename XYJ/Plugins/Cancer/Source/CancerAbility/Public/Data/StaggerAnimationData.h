#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraShakeBase.h"
#include "StaggerAnimationData.generated.h"

/**
 * 动画方向类型枚举
 */
UENUM(BlueprintType)
enum class EStaggerAnimationDirection : uint8
{
	FourDirection UMETA(DisplayName = "4向"),
	TwoDirection UMETA(DisplayName = "2向")
};

/**
 * 硬直动画配置结构体
 */
USTRUCT(BlueprintType)
struct CANCERABILITY_API FStaggerAnimationConfig : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "硬直动画ID")
	int32 StaggerMontageID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "硬直动画")
	TObjectPtr<UAnimMontage> StaggerMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "由攻击方决定硬直时长")
	bool bAttackerControlsDuration = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "动画方向类型")
	EStaggerAnimationDirection DirectionType = EStaggerAnimationDirection::FourDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "镜头抖动")
	TSubclassOf<UCameraShakeBase> CameraShake;
	FStaggerAnimationConfig()
	{
		StaggerMontageID = 0;
		bAttackerControlsDuration = true;
		DirectionType = EStaggerAnimationDirection::FourDirection;
	}
};

/**
 * 硬直动画模块类型枚举
 */
UENUM(BlueprintType)
enum class EStaggerType : uint8
{
	Normal UMETA(DisplayName = "普通"),
	KnockDown UMETA(DisplayName = "倒地连"),
	KnockFly UMETA(DisplayName = "浮空连")
};

UCLASS(BlueprintType)
class CANCERABILITY_API UStaggerAnimationDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "普通模块", Category = "硬直动画")
	TArray<FStaggerAnimationConfig> NormalAnimations;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "倒地连模块", Category = "硬直动画")
	TArray<FStaggerAnimationConfig> KnockDownAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "浮空连模块", Category = "硬直动画")
	TArray<FStaggerAnimationConfig> KnockFlyAnimations;

	FStaggerAnimationConfig FindClosestAnimation(const TArray<FStaggerAnimationConfig> Array, int32 AnimID) const;
	
	UFUNCTION(BlueprintCallable, Category = "硬直动画")
	FStaggerAnimationConfig FindStaggerAnimation(EStaggerType Module, int32 AnimationID,bool bExactMatch) const;
	
	UFUNCTION(BlueprintCallable, Category = "硬直动画")
	const TArray<FStaggerAnimationConfig>& GetAnimationsByModule(EStaggerType Module) const;
};
