#include "AI/BTTask_ExecuteAbility.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "CancerAbilitySystemComponent.h"

class UCancerAbilitySystemComponent;

UBTTask_ExecuteAbility::UBTTask_ExecuteAbility()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_ExecuteAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	if (ASC->TryActivateAbilitiesByTag(AbilityTag))
	{
		BoundAbility = ASC->GetAnimatingAbility();
		if (BoundAbility != nullptr)
		{
			AbilityCancelledHandle = BoundAbility->OnGameplayAbilityCancelled.AddUObject(
			this, &ThisClass::OnAbilityCancelled);
			AbilityEndedHandle = BoundAbility->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnd);
			return EBTNodeResult::InProgress;
		}
		else
		{
			UE_LOG(LogTemp, Warning , TEXT(" Ability Anim is NULL ! UBTTask_ExecuteAbility "))
		}
		
	}
	return EBTNodeResult::Failed;
}

void UBTTask_ExecuteAbility::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                            EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_ExecuteAbility::OnAbilityCancelled()
{
	if (BoundAbility.IsValid())
	{
		BoundAbility->OnGameplayAbilityCancelled.Remove(AbilityCancelledHandle);
		AbilityCancelledHandle.Reset();
		BoundAbility->OnGameplayAbilityEnded.Remove(AbilityEndedHandle);
		AbilityEndedHandle.Reset();
	}
	BoundAbility.Reset();
	if (CachedOwnerComp.IsValid())
	{
		FinishLatentTask(*CachedOwnerComp, bTreatCancelAsSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
	}
}

void UBTTask_ExecuteAbility::OnAbilityEnd(UGameplayAbility* GameplayAbility)
{
	if (BoundAbility.IsValid())
	{
		BoundAbility->OnGameplayAbilityCancelled.Remove(AbilityCancelledHandle);
		AbilityCancelledHandle.Reset();
		BoundAbility->OnGameplayAbilityEnded.Remove(AbilityEndedHandle);
		AbilityEndedHandle.Reset();
	}
	BoundAbility.Reset();
	if (CachedOwnerComp.IsValid())
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}
