#include "AI/BTTask_ExecuteMeleeCombo.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "CancerAbilitySystemComponent.h"
#include "AbilitySystem/CancerCombatAnimationAbility.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CancerCombo.h"

UBTTask_ExecuteMeleeCombo::UBTTask_ExecuteMeleeCombo()
{
	NodeName = TEXT("Execute MeleeCombo Ability");
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ExecuteMeleeCombo::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
                                                           uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;

	AActor* AvatarActor = nullptr;
	if (AAIController* AIOwner = OwnerComp.GetAIOwner())
	{
		AvatarActor = AIOwner->GetPawn();
	}
	else
	{
		AvatarActor = OwnerComp.GetOwner();
	}

	if (!IsValid(AvatarActor))
	{
		return EBTNodeResult::Failed;
	}

	UCancerAbilitySystemComponent* ASC = Cast<UCancerAbilitySystemComponent>(
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AvatarActor));
	if (!IsValid(ASC))
	{
		return EBTNodeResult::Failed;
	}
	CachedASC = ASC;

	FGameplayTag ChosenTag;
	if (AbilityTagWeights.Num() > 0)
	{
		float TotalWeight = 0.0f;
		for (const FAbilityTagWeight& Pair : AbilityTagWeights)
		{
			if (Pair.Weight > 0.0f)
			{
				TotalWeight += Pair.Weight;
			}
		}

		if (TotalWeight > 0.0f)
		{
			const float Pick = FMath::FRandRange(0.0f, TotalWeight);
			float Accum = 0.0f;
			for (const FAbilityTagWeight& Pair : AbilityTagWeights)
			{
				if (Pair.Weight <= 0.0f)
				{
					continue;
				}
				Accum += Pair.Weight;
				if (Pick <= Accum)
				{
					ChosenTag = Pair.AbilityTag;
					break;
				}
			}
		}
		if (!ChosenTag.IsValid())
		{
			for (const FAbilityTagWeight& Pair : AbilityTagWeights)
			{
				if (Pair.AbilityTag.IsValid())
				{
					ChosenTag = Pair.AbilityTag;
					break;
				}
			}
		}
	}
	if (ChosenTag.IsValid())
	{
		FGameplayEventData TriggerEventData;
		UCancerCombo* Combo = NewObject<UCancerCombo>();
		Combo->ComboTag = ChosenTag;
		TriggerEventData.OptionalObject = Combo;
		if (ASC->HandleGameplayEvent(AbilityEventTag, &TriggerEventData))
		{
			BoundAbility = ASC->GetAnimatingAbility();
			BindAnimEvents();
			ElapsedWaitTime = 0.0f;
			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTask_ExecuteMeleeCombo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                               EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

EBTNodeResult::Type UBTTask_ExecuteMeleeCombo::AbortTask(UBehaviorTreeComponent& OwnerComp,
                                                         uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_ExecuteMeleeCombo::OnAbilityEnd(UGameplayAbility* Ability)
{
	UnbindAnimEvents();
	if (CachedOwnerComp.IsValid())
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_ExecuteMeleeCombo::OnAbilityCancelled()
{
	UnbindAnimEvents();
	if (CachedOwnerComp.IsValid())
	{
		FinishLatentTask(*CachedOwnerComp, bTreatCancelAsSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
	}
}

void UBTTask_ExecuteMeleeCombo::BindAnimEvents()
{
	//绑定技能事件
	if (!IsValid(BoundAbility.Get()))
	{
		return;
	}

	// 绑定结束与取消事件，确保任务可靠收尾
	AbilityCancelledHandle = BoundAbility->OnGameplayAbilityCancelled.AddUObject(
		this, &ThisClass::OnAbilityCancelled);
	AbilityEndedHandle = BoundAbility->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnd);
}

void UBTTask_ExecuteMeleeCombo::UnbindAnimEvents()
{
	if (BoundAbility.IsValid())
	{
		BoundAbility->OnGameplayAbilityCancelled.Remove(AbilityCancelledHandle);
		AbilityCancelledHandle.Reset();
		BoundAbility->OnGameplayAbilityEnded.Remove(AbilityEndedHandle);
		AbilityEndedHandle.Reset();
	}
	BoundAbility.Reset();
}

void UBTTask_ExecuteMeleeCombo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                         float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// 若未成功绑定或没有可用的能力实例，尝试重新捕获；否则按超时结束避免挂起
	if (!CachedASC.IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!BoundAbility.IsValid())
	{
		BoundAbility = CachedASC->GetAnimatingAbility();
		if (BoundAbility.IsValid())
		{
			BindAnimEvents();
			return;
		}

		ElapsedWaitTime += DeltaSeconds;
		if (MaxWaitTime > 0.0f && ElapsedWaitTime >= MaxWaitTime)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
