#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Data/AICombatData.h"
#include "CancerAITypes.h"

#include "BTService_UpdateCombatState.generated.h"

/**
 * 更新战斗中状态
 */
UCLASS(DisplayName="更新AICombat状态")
class CANCERAI_API UBTService_UpdateCombatState : public UBTService
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = Cancer)
	TObjectPtr<class ACancerAIController> AIController;

	UPROPERTY(BlueprintReadOnly, Category = Cancer)
	TObjectPtr<class APawn> CharOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", DisplayName="战斗数据配置")
	TObjectPtr<class UAICombatData> CombatData;

	UPROPERTY(BlueprintReadOnly, Category = Cancer)
	float CombatAttackTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", DisplayName="战斗数据配置")
	EAICombatState CombatState{EAICombatState::EMeleeCombat};
	
    // 攻击触发时设置的AICombat状态Key
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta=(AllowedTypes="Enum"), DisplayName="AICombat状态Key")
    FBlackboardKeySelector AICombatState;

    // 运行时：当前攻击计时与锁定的间隔值
    UPROPERTY(Transient, BlueprintReadOnly, Category = Cancer)
    float AttackElapsedTime = 0.f;

    UPROPERTY(Transient, BlueprintReadOnly, Category = Cancer)
    float CurrentAttackInterval = 0.f;

public:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
