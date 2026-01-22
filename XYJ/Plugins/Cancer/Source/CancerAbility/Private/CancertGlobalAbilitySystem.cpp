#include "CancertGlobalAbilitySystem.h"
#include "AbilitySystemGlobals.h"
#include "CancerAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"
#include "CancerAbilityFunctionLibrary.h"
#include "GameplayTagsSettings.h"
#include "Blueprint/UserWidget.h"
#include "Components/CancerCombatAttributeComponent.h"
#include "Components/CancerHeroComponent.h"
#include "GameFramework/CancerDamageType.h"
#include "GameFramework/CancerWidgetEntry.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

TMap<FName,FGameplayTag> UCancerGlobalAbilitySystem::GlobalTagNameMap;
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
}

TStatId UCancerGlobalAbilitySystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(CancerGlobalAbilitySystem, STATGROUP_Tickables);
}


float UCancerGlobalAbilitySystem::GetRandomFloatInRange(float Min, float Max) const
{
	return RandomStream.FRandRange(Min, Max);
}

void UCancerGlobalAbilitySystem::K2_CombatSlomo(AActor* Attacker, AActor* Victim, const FCancerComboInfo& ComboInfo)
{
	if (!GetWorld())
	{
		return;
	}
	APawn* AttackerPawn = Cast<APawn>(Attacker);
	APlayerController* PC = AttackerPawn ? Cast<APlayerController>(AttackerPawn->GetController()) : nullptr;

	if (!PC)
	{
		return;
	}
	if (!PC->IsLocalController())
	{
		return;
	}
	// 清空上一次的状态
	if (TimeHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeHandle);
	}
	if (AActor* PrevAttacker = CachedAttacker.Get())
	{
		PrevAttacker->CustomTimeDilation = 1.f;
	}
	if (AActor* PrevVictim = CachedVictim.Get())
	{
		PrevVictim->CustomTimeDilation = 1.f;
	}
	CachedAttacker.Reset();
	CachedVictim.Reset();
	Elapsed = 0.f;

	PC->ClientStartCameraShake(ComboInfo.CameraShake);

	CachedAttacker = Attacker;
	CachedVictim = Victim;
	CurrentComboInfo = ComboInfo;
	const float D0 = FMath::Clamp(ComboInfo.SolmoCurve.GetRichCurveConst()->Eval(0.f), 0.01f, 10.f);
	if (ComboInfo.Type == ECancerType::Attacker && IsValid(Attacker))
	{
		Attacker->CustomTimeDilation = D0;
	}
	else if (ComboInfo.Type == ECancerType::Victim && IsValid(Victim))
	{
		Victim->CustomTimeDilation = D0;
	}
	else if (ComboInfo.Type == ECancerType::Both)
	{
		if (IsValid(Attacker))
		{
			Attacker->CustomTimeDilation = D0;
		}
		if (IsValid(Victim))
		{
			Victim->CustomTimeDilation = D0;
		}
	}
	if (TimeHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeHandle);
	}
	TimerDelegate.BindUFunction(this, FName("HandleSlomo"), Attacker, Victim, ComboInfo);
	GetWorld()->GetTimerManager().SetTimer(TimeHandle, TimerDelegate, FMath::Max(GetWorld()->GetDeltaSeconds(), 0.001f),
	                                       true);
}

FGameplayTag UCancerGlobalAbilitySystem::GetTagByName(const FName TagName)
{
	if (FGameplayTag* Tag =  GlobalTagNameMap.Find(TagName))
	{
		return *Tag;
	}
	const UGameplayTagsSettings* TagSettings = GetDefault<UGameplayTagsSettings>();
	FString ContextString;
	for (const auto TagTable : TagSettings->GameplayTagTableList)
	{
		UDataTable* TargetTagTable = Cast<UDataTable>( TagTable.TryLoad()) ;
		if (FGameplayTagTableRow* TagTableRow =  TargetTagTable->FindRow<FGameplayTagTableRow>(TagName,ContextString,false))
		{
			FGameplayTag ResultTag = UGameplayTagsManager::Get().RequestGameplayTag(TagTableRow->Tag);
			GlobalTagNameMap.Add(TagName, ResultTag);
			return ResultTag;
		}
	}
	return {};
}

void UCancerGlobalAbilitySystem::HandleSlomo(AActor* InAttacker, AActor* InVictim, const FCancerComboInfo& ComboInfo)
{
	if (!GetWorld())
	{
		return;
	}
	Elapsed += GetWorld()->GetDeltaSeconds();
	const float Alpha = FMath::Clamp(Elapsed / FMath::Max(CurrentComboInfo.SolmoTime, 0.0001f), 0.f, 1.f);
	const float Dilation = FMath::Clamp(CurrentComboInfo.SolmoCurve.GetRichCurveConst()->Eval(Alpha), 0.01f, 10.f);
	AActor* Attacker = CachedAttacker.Get();
	AActor* Victim = CachedVictim.Get();
	if (CurrentComboInfo.Type == ECancerType::Attacker && IsValid(Attacker))
	{
		Attacker->CustomTimeDilation = Dilation;
	}
	else if (CurrentComboInfo.Type == ECancerType::Victim && IsValid(Victim))
	{
		Victim->CustomTimeDilation = Dilation;
	}
	else if (CurrentComboInfo.Type == ECancerType::Both)
	{
		if (IsValid(Attacker))
		{
			Attacker->CustomTimeDilation = Dilation;
		}
		if (IsValid(Victim))
		{
			Victim->CustomTimeDilation = Dilation;
		}
	}
	if (Elapsed >= CurrentComboInfo.SolmoTime)
	{
		if (IsValid(Attacker))
		{
			Attacker->CustomTimeDilation = 1;
		}
		if (IsValid(Victim))
		{
			Victim->CustomTimeDilation = 1;
		}
		GetWorld()->GetTimerManager().ClearTimer(TimeHandle);
		CachedAttacker.Reset();
		CachedVictim.Reset();
		Elapsed = 0.f;
	}
}
