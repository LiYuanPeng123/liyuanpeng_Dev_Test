#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTTask_ExecuteMeleeCombo.generated.h"

class UCancerCombatAnimationAbility;
class UGameplayAbility;
class UCancerAbilitySystemComponent;
class UAbilityTask_PlayCombatAnimation;

USTRUCT(BlueprintType)
struct FAbilityTagWeight
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability", meta=(ClampMin="0"))
	float Weight = 0.0f;
};

UCLASS(DisplayName="执行连招")
class CANCERABILITY_API UBTTask_ExecuteMeleeCombo : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ExecuteMeleeCombo();

	// UBTTaskNode
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                            EBTNodeResult::Type TaskResult) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag AbilityEventTag;

	// 可配置的“连招技能标签+权重”列表；若不为空，则按权重进行概率抽取
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TArray<FAbilityTagWeight> AbilityTagWeights;

	UFUNCTION()
	void OnAbilityEnd(UGameplayAbility* Ability);

	UFUNCTION()
	void OnAbilityCancelled();

private:
	void BindAnimEvents();
	void UnbindAnimEvents();

private:
	// 可靠的结束/取消事件句柄
	FDelegateHandle AbilityEndedHandle;
	FDelegateHandle AbilityCancelledHandle;

	// 超时防挂起：若绑定失败或能力未能正确结束，按超时结束任务
	UPROPERTY(EditAnywhere, Category="Ability", meta=(ClampMin="0.0"))
	float MaxWaitTime = 5.0f;

	UPROPERTY(Transient)
	float ElapsedWaitTime = 0.0f;

	UPROPERTY(EditAnywhere, Category="Ability")
	bool bTreatCancelAsSuccess = true;

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	TWeakObjectPtr<UCancerAbilitySystemComponent> CachedASC;
	TWeakObjectPtr<UGameplayAbility> BoundAbility;
	TWeakObjectPtr<UAbilityTask_PlayCombatAnimation> BoundAnimTask;
};
