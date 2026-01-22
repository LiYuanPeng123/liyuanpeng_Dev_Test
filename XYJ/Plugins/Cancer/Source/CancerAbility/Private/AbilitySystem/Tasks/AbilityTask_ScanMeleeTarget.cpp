#include "AbilitySystem/Tasks/AbilityTask_ScanMeleeTarget.h"
#include "GameFramework/CancerMeleeScan.h"
#include "GameFramework/CancerDamageType.h"

UAbilityTask_ScanMeleeTarget::UAbilityTask_ScanMeleeTarget()
{
	bTickingTask = true;
}

void UAbilityTask_ScanMeleeTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	// 本帧全局唯一 Actor 集合：跨所有扫描实例按 Actor 去重
	TSet<TWeakObjectPtr<AActor>> UniqueActorsThisFrame;

	for (auto It = MeleeScans.CreateIterator(); It; ++It)
	{
		UCancerMeleeScan* MeleeScan = *It;
		if (IsValid(MeleeScan) && MeleeScan->IsValidLowLevelFast())
		{
			// Always perform scan each tick to avoid missing fast actions
			TArray<FHitResult> Hits;
			MeleeScan->ScanForTargets(Hits);

			// Per-actor cooldown for multi-hit limiting (HPS), without skipping scans
			bool bAllowMultipleHits = false;
			int32 HitsPerSecond = 0;
			if (MeleeScan->DamageType)
			{
				bAllowMultipleHits = MeleeScan->DamageType->DamageParameter.TranceInfo.bAllowMultipleHits;
				HitsPerSecond = MeleeScan->DamageType->DamageParameter.TranceInfo.Hits;
			}

			TArray<FHitResult> FilteredHits;
			FilteredHits.Reserve(Hits.Num());

			if (bAllowMultipleHits)
			{
				TMap<TWeakObjectPtr<AActor>, float>& CooldownsForScan = ActorCooldowns.FindOrAdd(MeleeScan);
				// Decay cooldowns
				for (auto& Pair : CooldownsForScan)
				{
					Pair.Value -= DeltaTime;
				}

				const int32 SafeHPS = FMath::Max(1, HitsPerSecond);
				const float Interval = 1.0f / static_cast<float>(SafeHPS);

				for (const FHitResult& Hit : Hits)
				{
					AActor* HitActor = Hit.GetActor();
					if (!HitActor)
					{
						continue;
					}
					float& ActorCD = CooldownsForScan.FindOrAdd(HitActor, 0.0f);
					if (ActorCD > 0.0f)
					{
						if (MeleeScan->bEnableDebug)
						{
							UE_LOG(LogTemp, Verbose, TEXT("[MeleeScan] 跳过对 %s 的重复命中，冷却剩余 %.3fs (HPS=%d)"),
							       *GetNameSafe(HitActor), ActorCD, SafeHPS);
						}
						continue; // Skip repeated hits within cooldown window
					}
					ActorCD = Interval; // Start cooldown for this actor
					FilteredHits.Add(Hit);
				}
			}
			else
			{
				// 单次命中：ScanForTargets 已在本帧内做过去重，直接转发
				FilteredHits = MoveTemp(Hits);
			}

			if (MeleeScan->bEnableDebug)
			{
				UE_LOG(LogTemp, Log, TEXT("[MeleeScan] 本次检测命中数量: %d, 过滤后: %d"), Hits.Num(), FilteredHits.Num());
			}

			// 二次过滤：针对“本帧全局”按 Actor 去重（避免多个扫描实例重复广播同一目标）
			TArray<FHitResult> UniqueHitsThisFrame;
			UniqueHitsThisFrame.Reserve(FilteredHits.Num());
			for (const FHitResult& Hit : FilteredHits)
			{
				AActor* HitActor = Hit.GetActor();
				if (!HitActor)
				{
					continue;
				}
				const TWeakObjectPtr<AActor> WeakActor(HitActor);
				if (UniqueActorsThisFrame.Contains(WeakActor))
				{
					if (MeleeScan->bEnableDebug)
					{
						UE_LOG(LogTemp, Verbose, TEXT("[MeleeScan] 本帧跨实例去重，跳过目标: %s"), *GetNameSafe(HitActor));
					}
					continue; // 本帧已由其他实例命中并准备广播
				}
				UniqueActorsThisFrame.Add(WeakActor);
				UniqueHitsThisFrame.Add(Hit);
			}

			// 仅在存在“本帧唯一”目标时进行广播
			if (ShouldBroadcastAbilityTaskDelegates() && UniqueHitsThisFrame.Num() > 0)
			{
				const FGameplayAbilityTargetDataHandle TargetData = StartLocation.MakeTargetDataHandleFromHitResults(
					Ability, UniqueHitsThisFrame);
				OnMeleeTargetsFound.Broadcast(MeleeScan, TargetData);
			}
		}
		else
		{
			if (MeleeScan && MeleeScan->bEnableDebug)
			{
				UE_LOG(LogTemp, Warning, TEXT("[MeleeScan] 移除失效的近战扫描实例: %s"), *GetNameSafe(MeleeScan));
			}
			ActorCooldowns.Remove(MeleeScan);
			It.RemoveCurrent();
		}
	}
}

UAbilityTask_ScanMeleeTarget* UAbilityTask_ScanMeleeTarget::CreateTask(UGameplayAbility* OwningAbility)
{
	UAbilityTask_ScanMeleeTarget* NewTask = NewAbilityTask<UAbilityTask_ScanMeleeTarget>(OwningAbility);

	NewTask->StartLocation.SourceAbility = OwningAbility;
	NewTask->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
	NewTask->StartLocation.SourceActor = OwningAbility->GetAvatarActorFromActorInfo();

	return NewTask;
}

void UAbilityTask_ScanMeleeTarget::Join(const UCancerMeleeScan* MeleeScan)
{
	if (!MeleeScans.Contains(MeleeScan))
	{
		UCancerMeleeScan* NonConstScan = const_cast<UCancerMeleeScan*>(MeleeScan);
		MeleeScans.Add(NonConstScan);
		// Initialize per-actor cooldown map
		ActorCooldowns.FindOrAdd(NonConstScan, TMap<TWeakObjectPtr<AActor>, float>());

		if (NonConstScan && NonConstScan->bEnableDebug)
		{
			bool bAllowMultipleHits = false;
			int32 HitsPerSecond = 0;
			if (NonConstScan->DamageType)
			{
				bAllowMultipleHits = NonConstScan->DamageType->DamageParameter.TranceInfo.bAllowMultipleHits;
				HitsPerSecond = NonConstScan->DamageType->DamageParameter.TranceInfo.Hits;
			}
			UE_LOG(LogTemp, Log, TEXT("[MeleeScan] 任务已加入扫描；允许重复命中=%s，HPS=%d"),
			       bAllowMultipleHits ? TEXT("是") : TEXT("否"), HitsPerSecond);
		}
	}
}
