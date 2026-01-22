#pragma once

#include "CoreMinimal.h"
#include "CancerAITypes.h"
#include "Engine/DataAsset.h"
#include "AICombatConfigAsset.generated.h"

/**
 * AI行为参数
 */
UCLASS()
class CANCERAI_API UAICombatConfigAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // 默认 AI 状态（无目标时）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", DisplayName="默认状态（无目标时）")
    EAIState DefaultState = EAIState::EWait;

    // 巡路类型（随机点/跟随样条等）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", DisplayName="寻路状态")
    EPatrolType DefaultPatrol = EPatrolType::ERandomPoint;

    // 最大允许离出生点距离（超过则返家）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", DisplayName="最大距离(超过该距离将返回初始点)")
    float MaxDis = 3500.f;
};
