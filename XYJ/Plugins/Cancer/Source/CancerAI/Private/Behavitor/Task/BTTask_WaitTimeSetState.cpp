
#include "Behavitor/Task/BTTask_WaitTimeSetState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"


UBTTask_WaitTimeSetState::UBTTask_WaitTimeSetState()
{
    NodeName = TEXT("等待设置状态");
    bNotifyTaskFinished = true; // 启用完成回调
}


void UBTTask_WaitTimeSetState::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

    if (TaskResult != EBTNodeResult::Succeeded)
    {
        return;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return;
    }

    if (bAIStateSet && !AIStateKey.SelectedKeyName.IsNone())
    {
        BlackboardComp->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(AIState));
    }

    if (bAICombatStateSet && !AICombatStateKey.SelectedKeyName.IsNone())
    {
        BlackboardComp->SetValueAsEnum(AICombatStateKey.SelectedKeyName, static_cast<uint8>(AICombatState));
    }
}
