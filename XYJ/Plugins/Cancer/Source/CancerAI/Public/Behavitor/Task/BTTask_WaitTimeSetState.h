#pragma once

#include "CoreMinimal.h"
#include "CancerAITypes.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BTTask_WaitTimeSetState.generated.h"

/**
 * 
 */
UCLASS(DisplayName="等待设置状态")
class CANCERAI_API UBTTask_WaitTimeSetState : public UBTTask_Wait
{
	GENERATED_BODY()

public:
    UBTTask_WaitTimeSetState();
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
    bool bAIStateSet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta= (EditCondition ="bAIStateSet"))
	EAIState AIState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	bool bAICombatStateSet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta= (EditCondition ="bAICombatStateSet"))
	EAICombatState AICombatState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta=(AllowedTypes="Enum"), DisplayName="AI状态Key")
	FBlackboardKeySelector AIStateKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta=(AllowedTypes="Enum"), DisplayName="AICombat状态Key")
	FBlackboardKeySelector AICombatStateKey;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
};
