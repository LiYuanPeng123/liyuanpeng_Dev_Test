// 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerPlayAnimationAbility.h"
#include "CombatAbility_Summon.generated.h"

/**
 * 召唤技能
 */
UCLASS()
class CANCERABILITY_API UCombatAbility_Summon : public UCancerPlayAnimationAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Debug测试")
	bool bSummonDebug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="召唤物", Meta=(EditCondition="bSummonDebug"))
	TSubclassOf<AActor> SummonActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="数量", Meta=(EditCondition="bSummonDebug"))
	int32 SummonDebugNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="相对生成位置", Meta=(EditCondition="bSummonDebug"))
	FVector SummonDebugVector;


	


	
};
