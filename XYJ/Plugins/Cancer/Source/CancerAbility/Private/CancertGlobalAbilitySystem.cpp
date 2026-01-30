#include "CancertGlobalAbilitySystem.h"
#include "AbilitySystemGlobals.h"
#include "CancerAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"
#include "CancerAbilityFunctionLibrary.h"
#include "GameplayTagsSettings.h"
#include "Components/CancerCombatAttributeComponent.h"
#include "GameFramework/CancerDamageType.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"

static TAutoConsoleVariable<int32> CVarCancerDebugSlomo(
	TEXT("Cancer.DebugSlomo"),
	0,
	TEXT("Show Debug Info for Cancer Slomo System.\n")
	TEXT("0: Off\n")
	TEXT("1: On"),
	ECVF_Scalability | ECVF_RenderThreadSafe
);

TMap<FName, FGameplayTag> UCancerGlobalAbilitySystem::GlobalTagNameMap;

void FGlobalAppliedAbilityList::AddToASC(TSubclassOf<UGameplayAbility> Ability, UCancerAbilitySystemComponent* ASC)
{
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		RemoveFromASC(ASC);
	}
	UGameplayAbility* AbilityCDO = Ability->GetDefaultObject<UGameplayAbility>();
	FGameplayAbilitySpec AbilitySpec(AbilityCDO);
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
	Handles.Add(ASC, AbilitySpecHandle);
}

void FGlobalAppliedAbilityList::RemoveFromASC(UCancerAbilitySystemComponent* ASC)
{
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		ASC->ClearAbility(*SpecHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedAbilityList::RemoveFromAll()
{
	for (auto& KVP : Handles)
	{
		if (KVP.Key != nullptr)
		{
			KVP.Key->ClearAbility(KVP.Value);
		}
	}
	Handles.Empty();
}

void FGlobalAppliedEffectList::AddToASC(TSubclassOf<UGameplayEffect> Effect, UCancerAbilitySystemComponent* ASC)
{
	if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		RemoveFromASC(ASC);
	}

	const UGameplayEffect* GameplayEffectCDO = Effect->GetDefaultObject<UGameplayEffect>();
	const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(
		GameplayEffectCDO, /*Level=*/ 1, ASC->MakeEffectContext());
	Handles.Add(ASC, GameplayEffectHandle);
}

void FGlobalAppliedEffectList::RemoveFromASC(UCancerAbilitySystemComponent* ASC)
{
	if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		ASC->RemoveActiveGameplayEffect(*EffectHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedEffectList::RemoveFromAll()
{
	for (auto& KVP : Handles)
	{
		if (KVP.Key != nullptr)
		{
			KVP.Key->RemoveActiveGameplayEffect(KVP.Value);
		}
	}
	Handles.Empty();
}

UCancerGlobalAbilitySystem::UCancerGlobalAbilitySystem()
{
}

void UCancerGlobalAbilitySystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)
{
	if ((Ability.Get() != nullptr) && (!AppliedAbilities.Contains(Ability)))
	{
		FGlobalAppliedAbilityList& Entry = AppliedAbilities.Add(Ability);
		for (UCancerAbilitySystemComponent* ASC : RegisteredASCs)
		{
			Entry.AddToASC(Ability, ASC);
		}
	}
}

void UCancerGlobalAbilitySystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)
{
	if ((Effect.Get() != nullptr) && (!AppliedEffects.Contains(Effect)))
	{
		FGlobalAppliedEffectList& Entry = AppliedEffects.Add(Effect);
		for (UCancerAbilitySystemComponent* ASC : RegisteredASCs)
		{
			Entry.AddToASC(Effect, ASC);
		}
	}
}

void UCancerGlobalAbilitySystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)
{
	if ((Ability.Get() != nullptr) && AppliedAbilities.Contains(Ability))
	{
		FGlobalAppliedAbilityList& Entry = AppliedAbilities[Ability];
		Entry.RemoveFromAll();
		AppliedAbilities.Remove(Ability);
	}
}

void UCancerGlobalAbilitySystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)
{
	if ((Effect.Get() != nullptr) && AppliedEffects.Contains(Effect))
	{
		FGlobalAppliedEffectList& Entry = AppliedEffects[Effect];
		Entry.RemoveFromAll();
		AppliedEffects.Remove(Effect);
	}
}

void UCancerGlobalAbilitySystem::RegisterASC(UCancerAbilitySystemComponent* ASC)
{
	check(ASC);

	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.AddToASC(Entry.Key, ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.AddToASC(Entry.Key, ASC);
	}

	RegisteredASCs.AddUnique(ASC);
}

void UCancerGlobalAbilitySystem::UnregisterASC(UCancerAbilitySystemComponent* ASC)
{
	check(ASC);
	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.RemoveFromASC(ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.RemoveFromASC(ASC);
	}

	RegisteredASCs.Remove(ASC);
}

void UCancerGlobalAbilitySystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UAbilitySystemGlobals::Get().InitGlobalData();

	InitializeStream();
}

void UCancerGlobalAbilitySystem::InitializeStream()
{
	RandomStream = FRandomStream();
	RandomStream.Initialize(FDateTime::Now().ToUnixTimestamp());
}

void UCancerGlobalAbilitySystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSlomo(DeltaTime);

	if (CVarCancerDebugSlomo.GetValueOnGameThread() > 0)
	{
		// 1. Display Applied Slomo List
		int32 Index = 0;
		GEngine->AddOnScreenDebugMessage(1000 + Index++, 0.0f, FColor::Yellow,
		                                 TEXT("========== Applied Slomo Tasks =========="));

		for (const auto& Pair : AppliedSlomo)
		{
			const FAppliedSlomoHandle& Handle = Pair.Value;

			// Calculate current dilation for debug display
			const float MaxTime = FMath::Max(Handle.ComboInfo.SolmoTime, 0.0001f);
			const float Alpha = FMath::Clamp(Handle.ElapsedTime / MaxTime, 0.f, 1.f);
			float CurrentDilation = 1.0f;
			if (const FRichCurve* Curve = Handle.ComboInfo.SolmoCurve.GetRichCurveConst())
			{
				CurrentDilation = FMath::Clamp(Curve->Eval(Alpha), 0.01f, 10.f);
			}

			FString TypeStr;
			switch (Handle.ComboInfo.Type)
			{
			case ECancerType::Attacker: TypeStr = TEXT("Attacker");
				break;
			case ECancerType::Victim: TypeStr = TEXT("Victim");
				break;
			case ECancerType::Both: TypeStr = TEXT("Both");
				break;
			case ECancerType::Global: TypeStr = TEXT("Global");
				break;
			case ECancerType::GlobalExceptPlayer: TypeStr = TEXT("GlobalExceptPlayer");
				break;
			default: TypeStr = TEXT("Unknown");
				break;
			}

			FString DebugMsg = FString::Printf(
				TEXT("[Slomo] Attacker: %s | Victim: %s | Type: %s | Time: %.2f/%.2f | Dilation: %.2f"),
				Handle.Attacker ? *Handle.Attacker->GetName() : TEXT("None"),
				Handle.Victim ? *Handle.Victim->GetName() : TEXT("None"),
				*TypeStr,
				Handle.ElapsedTime,
				Handle.ComboInfo.SolmoTime,
				CurrentDilation);

			GEngine->AddOnScreenDebugMessage(1000 + Index, 0.0f, FColor::Yellow, DebugMsg);
			Index++;
		}

		GEngine->AddOnScreenDebugMessage(1000 + Index++, 0.0f, FColor::Yellow,
		                                 TEXT("========================================="));

		// 2. Display Global & Actor Time Dilation (Blue Line)
		float GlobalDilation = 1.0f;
		if (GetWorld() && GetWorld()->GetWorldSettings())
		{
			GlobalDilation = GetWorld()->GetWorldSettings()->TimeDilation;
		}

		FString GlobalMsg = FString::Printf(TEXT("[ RESULT ] GLOBAL: %.2f"), GlobalDilation);

		// Collect unique affected actors
		TSet<AActor*> AffectedActors;
		for (const auto& Pair : AppliedSlomo)
		{
			if (Pair.Value.Attacker) AffectedActors.Add(Pair.Value.Attacker);
			if (Pair.Value.Victim) AffectedActors.Add(Pair.Value.Victim);
		}

		for (AActor* Actor : AffectedActors)
		{
			if (IsValid(Actor))
			{
				GlobalMsg += FString::Printf(TEXT(" | %s: %.2f"), *Actor->GetName(), Actor->CustomTimeDilation);
			}
		}

		// Show Result with a box-like separator
		GEngine->AddOnScreenDebugMessage(2000, 0.0f, FColor::Blue,
		                                 TEXT("--------------------------------------------------"));
		GEngine->AddOnScreenDebugMessage(2001, 0.0f, FColor::Cyan, GlobalMsg);
		GEngine->AddOnScreenDebugMessage(2002, 0.0f, FColor::Blue,
		                                 TEXT("--------------------------------------------------"));
	}
}


void UCancerGlobalAbilitySystem::UpdateSlomo(float DeltaTime)
{
	if (AppliedSlomo.IsEmpty())
	{
		return;
	}

	// 矫正 DeltaTime，确保在全局时间缩放时，顿帧效果的流逝仍基于真实时间（或接近真实时间）
	const float GlobalDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	const float RealDeltaTime = (GlobalDilation > KINDA_SMALL_NUMBER) ? (DeltaTime / GlobalDilation) : DeltaTime;

	// 1. 收集数据
	TMap<AActor*, float> TargetDilations;
	float TargetGlobalDilation = 1.0f;
	bool bHasGlobalEffect = false;

	// 收集震动偏移
	// Key: Actor, Value: Accumulate Offset
	TMap<AActor*, FVector> MeshOffsets;

	// 收集后期强度 (取最大值)
	float TargetPPIntensity = 0.0f;

	for (auto It = AppliedSlomo.CreateIterator(); It; ++It)
	{
		FAppliedSlomoHandle& Handle = It.Value();

		// 累加经过的时间 (使用矫正后的 DeltaTime)
		Handle.ElapsedTime += RealDeltaTime;

		// 计算当前进度的曲线值
		const float MaxTime = FMath::Max(Handle.ComboInfo.SolmoTime, 0.0001f);
		const float Alpha = FMath::Clamp(Handle.ElapsedTime / MaxTime, 0.f, 1.f);
		const float CurrentDilation = FMath::Clamp(Handle.ComboInfo.SolmoCurve.GetRichCurveConst()->Eval(Alpha), 0.01f,
		                                           10.f);

		// 计算受击震动 ---
		if (Handle.ComboInfo.bEnableMeshShake)
		{
			// 从曲线获取当前时刻的震动强度 (0~1)
			const float ShakeScale = Handle.ComboInfo.ShakeCurve.GetRichCurveConst()->Eval(Alpha);
			if (FMath::Abs(ShakeScale) > KINDA_SMALL_NUMBER)
			{
				// 生成随机震动向量
				FVector RandomOffset = FMath::VRand() * Handle.ComboInfo.ShakeAmplitude * ShakeScale;

				auto ApplyShake = [&](AActor* Actor)
				{
					if (IsValid(Actor))
					{
						if (FVector* Found = MeshOffsets.Find(Actor))
						{
							*Found += RandomOffset;
						}
						else
						{
							MeshOffsets.Add(Actor, RandomOffset);
						}
					}
				};

				if (Handle.ComboInfo.Type == ECancerType::Victim || Handle.ComboInfo.Type == ECancerType::Both)
				{
					ApplyShake(Handle.Victim);
				}
				// 通常 Attacker 不需要震动，除非特殊需求
			}
		}

		// 收集后期特效 ---
		if ((Handle.ComboInfo.Type == ECancerType::Global || Handle.ComboInfo.Type == ECancerType::GlobalExceptPlayer)
			&& Handle.ComboInfo.bEnablePostProcess)
		{
			// 简单起见，这里直接使用配置的强度，配合时间淡入淡出可以做得更好
			// 这里假设曲线值越低（越慢），后期越强，或者直接由 PostProcessIntensity 控制
			TargetPPIntensity = FMath::Max(TargetPPIntensity, Handle.ComboInfo.PostProcessIntensity);
		}

		// 辅助Lambda：尝试更新目标的最小Dilation
		auto ApplyToTarget = [&](AActor* Actor)
		{
			if (IsValid(Actor))
			{
				if (float* Found = TargetDilations.Find(Actor))
				{
					*Found = FMath::Min(*Found, CurrentDilation);
				}
				else
				{
					TargetDilations.Add(Actor, CurrentDilation);
				}
			}
		};

		// 根据类型分发
		const ECancerType Type = Handle.ComboInfo.Type;

		if (Type == ECancerType::Attacker || Type == ECancerType::Both)
		{
			ApplyToTarget(Handle.Attacker);
		}

		if (Type == ECancerType::Victim || Type == ECancerType::Both)
		{
			ApplyToTarget(Handle.Victim);
		}

		if (Type == ECancerType::Global)
		{
			bHasGlobalEffect = true;
			TargetGlobalDilation = FMath::Min(TargetGlobalDilation, CurrentDilation);
		}

		if (Type == ECancerType::GlobalExceptPlayer)
		{
			bHasGlobalEffect = true;
			TargetGlobalDilation = FMath::Min(TargetGlobalDilation, CurrentDilation);

			// 找到本地玩家，强制将其速度补偿为 (1 / Global)
			// 注意：这只是一个建议值，如果玩家自己身上也有 Attacker/Victim 效果，
			// 根据 Min 规则，它可能会被 Attacker/Victim 的更小值覆盖。
			// 但如果是 GlobalExceptPlayer，我们的意图通常是让玩家“免疫”全局减速，所以这里我们给一个 High 值
			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				if (APawn* PlayerPawn = PC->GetPawn())
				{
					// 我们记录一个特殊的标记或直接在这里计算补偿值
					// 补偿值 = 1.0 / TargetGlobalDilation
					// 但由于循环还没结束，TargetGlobalDilation 可能会变，所以我们需要先收集 GlobalExceptPlayer 的意图
					// 简单起见，我们在循环结束后统一处理 Player 的补偿
				}
			}
		}
	}

	// 2. 特殊处理：如果有 GlobalExceptPlayer，我们需要计算对 Player 的补偿
	// 补偿逻辑：PlayerFinal = PlayerBase * Global = 1.0  => PlayerBase = 1.0 / Global
	bool bShouldCompensatePlayer = false;
	for (const auto& Pair : AppliedSlomo)
	{
		if (Pair.Value.ComboInfo.Type == ECancerType::GlobalExceptPlayer)
		{
			bShouldCompensatePlayer = true;
			break;
		}
	}

	if (bShouldCompensatePlayer && TargetGlobalDilation > 0.001f)
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APawn* PlayerPawn = PC->GetPawn())
			{
				float CompensatedDilation = 1.0f / TargetGlobalDilation;

				// 再次应用 Min 规则：如果 Player 身上还有 Attacker=0.5 的效果，
				// 那么 Compensated(10.0) 和 Attacker(0.5) 取 Min => 0.5。
				// 这符合逻辑：玩家虽然免疫了全局减速，但如果自己被打中了（Victim），还是会减速。
				if (float* Found = TargetDilations.Find(PlayerPawn))
				{
					// 这里的逻辑有点微妙：
					// 如果 TargetDilations 里已经有值，说明 Player 被 Attacker/Victim 命中了。
					// 此时我们应该保持那个更小的值，还是说 GlobalExceptPlayer 具有最高优先级？
					// 保持 Min 规则通常更稳健。
					// 但要注意：如果 TargetDilations 里没有值，我们需要 Add。
				}
				else
				{
					TargetDilations.Add(PlayerPawn, CompensatedDilation);
				}
			}
		}
	}

	// 3. 应用 Actor 级别的时间膨胀
	for (const auto& Pair : TargetDilations)
	{
		if (IsValid(Pair.Key))
		{
			Pair.Key->CustomTimeDilation = Pair.Value;
		}
	}

	// 3. 应用 Global 级别的时间膨胀
	if (bHasGlobalEffect)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TargetGlobalDilation);
	}

	// 4.应用模型震动 ---
	for (const auto& Pair : MeshOffsets)
	{
		if (ACharacter* Char = Cast<ACharacter>(Pair.Key))
		{
			if (USkeletalMeshComponent* Mesh = Char->GetMesh())
			{
				Mesh->AddLocalOffset(Pair.Value);
			}
		}
	}

	// 5. 应用后期特效 ---
	// 这里可以查找场景中的 PostProcessVolume 并设置参数，或者通过 MPC (Material Parameter Collection) 驱动
	if (TargetPPIntensity > 0.f)
	{
		// 示例：设置 MPC 参数，让材质去处理色差/模糊
		// UMaterialParameterCollectionInstance* Instance = GetWorld()->GetParameterCollectionInstance(MyMPC);
		// Instance->SetScalarParameterValue(FName("SlomoIntensity"), TargetPPIntensity);
	}
}

FGuid UCancerGlobalAbilitySystem::RegisterSlomo(AActor* Attacker, AActor* Victim, const FCancerComboInfo& ComboInfo)
{
	// 规则优化：区分主角和AI
	// 如果是战斗类型的顿帧（Attacker/Victim/Both），我们要求攻击发起者必须是玩家控制的角色。
	// 原因：通常只有主角的攻击才需要这种强烈的顿帧（卡肉）反馈。AI 的攻击通常不需要让 AI 自己变慢，也不需要让受击者变慢（除非是 Global 效果）。

	if (ComboInfo.Type == ECancerType::Attacker || ComboInfo.Type == ECancerType::Victim || ComboInfo.Type ==
		ECancerType::Both || ComboInfo.Type == ECancerType::GlobalExceptPlayer)
	{
		bool bIsPlayerAttacker = false;
		if (Attacker)
		{
			if (APawn* Pawn = Cast<APawn>(Attacker))
			{
				if (Pawn->IsPlayerControlled())
				{
					bIsPlayerAttacker = true;
				}
			}
		}

		// 如果 Attacker 不是玩家，直接跳过注册
		if (!bIsPlayerAttacker)
		{
			return FGuid();
		}
	}


	if (Attacker)
	{
		if (APawn* AttackerPawn = Cast<APawn>(Attacker))
		{
			if (APlayerController* PC = Cast<APlayerController>(AttackerPawn->GetController()))
			{
				if (PC->IsLocalController() && ComboInfo.CameraShake)
				{
					PC->ClientStartCameraShake(ComboInfo.CameraShake);
				}
			}
		}
	}

	if (ComboInfo.SolmoTime <= 0.f)
	{
		return FGuid();
	}
	FAppliedSlomoHandle SlomoHandle(ComboInfo, Attacker, Victim);
	if (!SlomoHandle.IsValid()) return {};
	auto Guid = SlomoHandle.GetGuid();
	AppliedSlomo.Add(SlomoHandle.GetGuid(), SlomoHandle);

	if (ComboInfo.SolmoTime > 0.f)
	{
		FTimerHandle TimerHandle;
		const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(
			this, &ThisClass::RemoveSlomo, Guid);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, ComboInfo.SolmoTime, false);

		if (TimerHandle.IsValid())
		{
			SlomoTimerHandles.Add(Guid, TimerHandle);
		}
	}

	return Guid;
}

void UCancerGlobalAbilitySystem::RemoveSlomo(FGuid Guid)
{
	if (FAppliedSlomoHandle* Handle = AppliedSlomo.Find(Guid))
	{
		// 辅助函数：尝试重置 Actor 的时间膨胀
		// 只有当该 Actor 不再受到任何其他 Slomo 效果影响时，才将其重置为 1.0
		auto TryResetActor = [&](AActor* Actor)
		{
			if (!IsValid(Actor))
			{
				return;
			}

			bool bStillAffected = false;
			for (const auto& Pair : AppliedSlomo)
			{
				// 跳过当前正在移除的 Handle
				if (Pair.Key == Guid)
				{
					continue;
				}

				const FAppliedSlomoHandle& OtherHandle = Pair.Value;
				const ECancerType OtherType = OtherHandle.ComboInfo.Type;

				// 检查其他 Handle 是否也影响了这个 Actor
				if (OtherType == ECancerType::Attacker || OtherType == ECancerType::Both)
				{
					if (OtherHandle.Attacker == Actor)
					{
						bStillAffected = true;
						break;
					}
				}
				if (OtherType == ECancerType::Victim || OtherType == ECancerType::Both)
				{
					if (OtherHandle.Victim == Actor)
					{
						bStillAffected = true;
						break;
					}
				}
			}

			if (!bStillAffected)
			{
				Actor->CustomTimeDilation = 1.f;
			}
		};

		if (Handle->ComboInfo.Type == ECancerType::Attacker || Handle->ComboInfo.Type == ECancerType::Both)
		{
			TryResetActor(Handle->Attacker);
		}
		if (Handle->ComboInfo.Type == ECancerType::Victim || Handle->ComboInfo.Type == ECancerType::Both)
		{
			TryResetActor(Handle->Victim);
		}

		// 如果移除的是 Global 或 GlobalExceptPlayer 类型的 Slomo
		// 且当前已经没有其他 Global/GlobalExceptPlayer Slomo 了，尝试恢复全局时间
		if (Handle->ComboInfo.Type == ECancerType::Global || Handle->ComboInfo.Type == ECancerType::GlobalExceptPlayer)
		{
			bool bHasOtherGlobal = false;
			for (const auto& Pair : AppliedSlomo)
			{
				if (Pair.Key != Guid &&
					(Pair.Value.ComboInfo.Type == ECancerType::Global || Pair.Value.ComboInfo.Type ==
						ECancerType::GlobalExceptPlayer))
				{
					bHasOtherGlobal = true;
					break;
				}
			}

			if (!bHasOtherGlobal)
			{
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

				// 同时也需要恢复 Player 的补偿值（如果存在）
				if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
				{
					if (APawn* PlayerPawn = PC->GetPawn())
					{
						// 注意：这里我们不能简单地设为 1.0，因为 Player 可能还受 Attacker/Victim 影响
						// 最好的办法是让下一帧 UpdateSlomo 自动处理。
						// 但由于 UpdateSlomo 依赖 AppliedSlomo，如果我们现在移除 Handle，
						// 下一帧 UpdateSlomo 自然会计算出正确的值。
						// 唯一的问题是：如果这是最后一个 Slomo，UpdateSlomo 会直接 return，
						// 所以我们需要在这里做一次清理，或者让 UpdateSlomo 即使 Empty 也能跑一次清理逻辑（有点浪费）。
						// 
						// 采用现有的 TryResetActor 逻辑即可，因为它会检查是否还受其他影响。
						TryResetActor(PlayerPawn);
					}
				}
			}
		}

		AppliedSlomo.Remove(Guid);
	}

	if (SlomoTimerHandles.Contains(Guid))
	{
		FTimerHandle TimerHandle = SlomoTimerHandles.FindAndRemoveChecked(Guid);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
}

TStatId UCancerGlobalAbilitySystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(CancerGlobalAbilitySystem, STATGROUP_Tickables);
}


float UCancerGlobalAbilitySystem::GetRandomFloatInRange(float Min, float Max) const
{
	return RandomStream.FRandRange(Min, Max);
}

FGameplayTag UCancerGlobalAbilitySystem::GetTagByName(const FName TagName)
{
	if (FGameplayTag* Tag = GlobalTagNameMap.Find(TagName))
	{
		return *Tag;
	}
	const UGameplayTagsSettings* TagSettings = GetDefault<UGameplayTagsSettings>();
	FString ContextString;
	for (const auto TagTable : TagSettings->GameplayTagTableList)
	{
		UDataTable* TargetTagTable = Cast<UDataTable>(TagTable.TryLoad());
		if (FGameplayTagTableRow* TagTableRow = TargetTagTable->FindRow<FGameplayTagTableRow>(
			TagName, ContextString, false))
		{
			FGameplayTag ResultTag = UGameplayTagsManager::Get().RequestGameplayTag(TagTableRow->Tag);
			GlobalTagNameMap.Add(TagName, ResultTag);
			return ResultTag;
		}
	}
	return {};
}
