#include "Behavitor/BTService_UpdateCombatState.h"
#include "CancerAIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTService_UpdateCombatState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AIController = Cast<ACancerAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (AIController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UStateUpdateBTService get controller failed"));
		return;
	}
	CharOwner = AIController->GetPawn();
	if (CharOwner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPawn() is NULL - UStateUpdateBTService"));
		AIController->Destroy();
		return;
	}
	if (!BB)
	{
		return;
	}

	// 若无目标，重置计时与当前间隔
	if (!AIController->HasTarget())
	{
		AttackElapsedTime = 0.f;
		CurrentAttackInterval = 0.f;
		CombatAttackTime = 0.f;
		return;
	}

	// 锁定一次攻击间隔，避免每Tick随机导致抖动
	if (CurrentAttackInterval <= 0.f)
	{
		float IntervalMultiplier = 1.0f;
		float BaseInterval = 1.0f;

		if (CombatData)
		{
			const FAttackIntervalSettings& Settings = CombatData->AttackInterval;
			if (Settings.bUseRandomRange)
			{
				const float MinV = FMath::Max(0.0f, FMath::Min(Settings.RandomMin, Settings.RandomMax));
				const float MaxV = FMath::Max(Settings.RandomMin, Settings.RandomMax);
				BaseInterval = FMath::FRandRange(MinV, MaxV);
			}
			else
			{
				BaseInterval = FMath::Max(0.0f, Settings.FixedValue);
			}
		}

		if (CombatData && CombatData->AttackIntervalMultipliers.Num() > 0)
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharOwner))
			{
				FGameplayTagContainer OwnedTags;
				ASC->GetOwnedGameplayTags(OwnedTags);

				float MinMul = IntervalMultiplier;
				for (const FAttackIntervalTagMultiplier& TM : CombatData->AttackIntervalMultipliers)
				{
					if (!TM.Tag.IsValid())
					{
						continue;
					}
					bool bHasTag = false;
					for (const FGameplayTag& OwnedTag : OwnedTags)
					{
						if (OwnedTag.MatchesTag(TM.Tag))
						{
							bHasTag = true;
							break;
						}
					}
					if (bHasTag)
					{
						const float ValidMul = FMath::Max(0.01f, TM.Multiplier);
						MinMul = FMath::Min(MinMul, ValidMul);
					}
				}
				IntervalMultiplier = MinMul;
			}
		}

		CurrentAttackInterval = BaseInterval * IntervalMultiplier;
		CombatAttackTime = CurrentAttackInterval;
	}

	// 累计时间并在达到间隔时触发攻击状态
	AttackElapsedTime += DeltaSeconds;

	if (AttackElapsedTime >= FMath::Max(CurrentAttackInterval, 0.0f))
	{
		if (AICombatState.SelectedKeyName != NAME_None)
		{
			BB->SetValueAsEnum(AICombatState.SelectedKeyName, static_cast<uint8>(CombatState));
		}

		// 重置一次循环，下一轮重新锁定间隔
		AttackElapsedTime = 0.f;
		CurrentAttackInterval = 0.f;
	}
}
