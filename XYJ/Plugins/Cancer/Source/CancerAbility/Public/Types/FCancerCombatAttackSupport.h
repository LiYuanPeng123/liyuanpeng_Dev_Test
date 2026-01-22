#pragma once

#include "CoreMinimal.h"
#include "CancerNativeGamePlayTag.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "AbilitySystem/Interfaces/CombatAttackAbilityInterface.h"
#include "AbilitySystem/Tasks/AbilityTask_ScanMeleeTarget.h"
#include "Animation/AnimNotifyState_MeleeScan.h"
#include "GameFramework/Actor.h"

class UCancerGameplayAbility;

struct FCancerCombatAttackSupport
{
	/**
	 * Handles the main events for when an event is received by an Attack Ability:
	 * Melee Scan Start, Melee Scan Stop and Launch Projectile.
	 */
	static void HandleEventReceived(UCancerGameplayAbility* Ability, const FGameplayEventData& Payload)
	{
		ICombatAttackAbilityInterface* AttackAbility = Cast<ICombatAttackAbilityInterface>(Ability);
		if (AttackAbility == nullptr)
		{
			return;
		}

		if (Payload.EventTag == Tag_Combat_Event_MeleeScan_Start)
		{
			const UCancerMeleeScan* Scan = GetMeleeScan(Payload);
			if (ensureMsgf(IsValid(Scan) && Scan->HasValidScanData(), TEXT("Missing or invalid Melee Scan.")))
			{
				UAbilityTask_ScanMeleeTarget* MeleeScanTask = AttackAbility->GetMeleeScanTask();
				if (IsValid(MeleeScanTask) && MeleeScanTask->IsActive())
				{
					MeleeScanTask->Join(Scan);
				}
				else
				{
					Ability->FinishLatentTask(MeleeScanTask);
					AttackAbility->InitializeMeleeScanTask(Scan);
				}
			}
		}
		else if (Payload.EventTag == Tag_Combat_Event_MeleeScan_Stop)
		{
			UAbilityTask_ScanMeleeTarget* MeleeScanTask = AttackAbility->GetMeleeScanTask();
			Ability->FinishLatentTask(MeleeScanTask);
		}
	}

	static const UCancerMeleeScan* GetMeleeScan(const FGameplayEventData& Payload)
	{
		const UCancerMeleeScan* Scan = Cast<UCancerMeleeScan>(Payload.OptionalObject);
		if (!IsValid(Scan))
		{
			Scan = Cast<UCancerMeleeScan>(Payload.OptionalObject2);
		}

		return Scan;
	}
	
};
