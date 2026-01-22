#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AICombatData.generated.h"


USTRUCT(BlueprintType)
struct FAttackIntervalSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Interval", DisplayName="是否范围随机")
	bool bUseRandomRange = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Interval", meta=(ClampMin=0.0), DisplayName="随机最小值")
	float RandomMin = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Interval", meta=(ClampMin=0.0), DisplayName="随机最大值")
	float RandomMax = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Interval", meta=(ClampMin=0.0), DisplayName="固定值")
	float FixedValue = 2.f;
};


USTRUCT(BlueprintType)
struct FAttackIntervalTagMultiplier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Interval", DisplayName="Tag")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Interval", meta=(ClampMin=0.01), DisplayName="乘数")
	float Multiplier = 1.0f;
};


UCLASS()
class CANCERAI_API UAICombatData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="进入攻击状态的频率", DisplayName="攻击间隔配置")
	FAttackIntervalSettings AttackInterval;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="进入攻击状态的频率", DisplayName="Tag间隔乘数")
	TArray<FAttackIntervalTagMultiplier> AttackIntervalMultipliers;
};
