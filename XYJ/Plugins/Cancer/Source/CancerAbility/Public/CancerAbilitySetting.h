#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "CancerAbilitySetting.generated.h"

UCLASS(config=Game, DefaultConfig, DisplayName="Cancer技能")
class CANCERABILITY_API UCancerAbilitySetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static UCancerAbilitySetting* Get();

	UPROPERTY(Config, EditAnywhere, DisplayName="近战扫描绘制时长")
	float MeleeScanDebugDuration = 3.0f;

    UPROPERTY(Config, EditAnywhere, DisplayName="近战检测通道")
    TArray<TEnumAsByte<ECollisionChannel>> MeleeScanChannels;

    UPROPERTY(Config, EditAnywhere, DisplayName="发射物检测通道")
    TEnumAsByte<ECollisionChannel> ProjectileChannel;

    UPROPERTY(Config, EditAnywhere, DisplayName="自动吸附检测通道")
    TArray<TEnumAsByte<ECollisionChannel>> AutoAdsorptionChannels;
	
	UPROPERTY(Config, EditAnywhere, DisplayName="默认伤害GE")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(Config, EditAnywhere, DisplayName="通用单次GE")
	TSubclassOf<class UGameplayEffect> GeneralEffectClass;
	
	UPROPERTY(Config, EditAnywhere, DisplayName="通用持续性GE")
	TSubclassOf<class UGameplayEffect> GeneralPeriodEffectClass;
	
	UPROPERTY(Config,BlueprintReadWrite, EditAnywhere, DisplayName="弹反冲击力",Category="弹反", meta = (ClampMin = "0"))
	int32 PerfectBlockImpactForce = 1;
	UPROPERTY(Config,BlueprintReadWrite, EditAnywhere, DisplayName="弹反硬直动画ID" ,Category="弹反")
	int32 PerfectBlockStaggerAnimationID = 0;
	UPROPERTY(Config,BlueprintReadWrite, EditAnywhere, DisplayName="弹反硬直时长" ,Category="弹反", meta = (ClampMin = "0.0"))
	float PerfectBlockHardDuration = 0.0f;
	UPROPERTY(Config,BlueprintReadWrite, EditAnywhere, DisplayName="弹反击退距离",Category="弹反", meta = (ClampMin = "0.0"))
	float PerfectBlockKnockbackDistance = 0.0f;

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
