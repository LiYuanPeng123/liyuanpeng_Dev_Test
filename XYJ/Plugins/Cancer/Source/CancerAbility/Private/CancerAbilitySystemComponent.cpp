#include "CancerAbilitySystemComponent.h"
#include "CancerAssetManager.h"
#include "CancerCoreFunctionLibrary.h"
#include "CancerCoreSetting.h"
#include "CancerInputFunctionLibrary.h"
#include "CancerNativeGamePlayTag.h"
#include "CancertGlobalAbilitySystem.h"
#include "Algo/StableSort.h"
#include "AbilitySystem/Attributes/CancerCombatSet.h"
#include "Components/CancerHeroComponent.h"
#include "Components/CancerMovementComponent.h"
#include "Data/CancerCombatData.h"
#include "Engine/DataTable.h"
#include"Abilities/GameplayAbility.h"
#include "AbilitySystem/Attributes/CancerProgressionSet.h"
#include "Chosse/CancerChooserLibrary.h"
#include "Components/CancerInputManagerComponent.h"
#include "VisualLogger/VisualLogger.h"
UE_DISABLE_OPTIMIZATION

TArray<FCancerActiveAbilityInfo> UCancerAbilitySystemComponent::GetAllAbilitiesInfo()
{
	//技能优先级被覆盖 为绿色 普通为白色（独立技能优先级为-1）没有配置优先级为红色
	TArray<FCancerActiveAbilityInfo> AllAbilitiesInfo;
	for (auto Pair : AbilitiesDefMap)
	{
		FCancerActiveAbilityInfo Info;
		Info.AbilityTag = Pair.Value.AbilityDef.AbilityTag; // Use stored tag since key is hash
		bool Override;
		auto AbilityPriority = GetAbilityPriority(Info.AbilityTag, Override);
		Info.Priority = AbilityPriority.ActivationGroup ==
		                ECancerAbilityGroup::Independent
			                ? -1
			                : AbilityPriority.Priority;
		Info.Color = Override ? FColor::Green : FColor::White;
		AllAbilitiesInfo.Add(Info);
	}
	return AllAbilitiesInfo;
}

UCancerAbilitySystemComponent::UCancerAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CombatData = nullptr;
}

void UCancerAbilitySystemComponent::BlockAbilitiesTags(const FGameplayTagContainer& Tags)
{
	BlockAbilitiesWithTags(Tags);
}

void UCancerAbilitySystemComponent::UnBlockAbilitiesTags(const FGameplayTagContainer& Tags)
{
	UnBlockAbilitiesWithTags(Tags);
}

void UCancerAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer,
                                                               TArray<UGameplayAbility*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer,
	                                                    AbilitiesToActivate, false);

	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			ActiveAbilities.Add(ActiveAbility);
		}
	}
}

void UCancerAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (const UCancerGameplayAbility* CancerAbilityCDO = Cast<UCancerGameplayAbility>(AbilitySpec.Ability))
		{
			CancerAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
		}
	}
}


void UCancerAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UCancerGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<UCancerGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->UnregisterASC(this);
	}
	if (auto InputManger = UCancerInputFunctionLibrary::GetCancerInputManagerComponent(GetOwner()))
	{
		InputManger->OnReleased.RemoveDynamic(this, &ThisClass::AbilityInputTagReleased);
		InputManger->OnPressed.RemoveDynamic(this, &ThisClass::AbilityInputTagPressed);
	}
	Super::EndPlay(EndPlayReason);
}

void UCancerAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	//check(ActorInfo);
	check(InOwnerActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (UCancerGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<UCancerGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->RegisterASC(this);
	}

	TryActivateAbilitiesOnSpawn();
}

void UCancerAbilitySystemComponent::BindToInputManager()
{
	if (auto InputManager = UCancerInputFunctionLibrary::GetCancerInputManagerComponent(GetOwner()))
	{
		InputManager->OnReleased.AddDynamic(this, &ThisClass::AbilityInputTagReleased);
		InputManager->OnPressed.AddDynamic(this, &ThisClass::AbilityInputTagPressed);
		InputManager->OnTriggered.AddDynamic(this, &ThisClass::AbilityInputTagTriggered);
	}
}


void UCancerAbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UCancerAbilitySystemComponent::OnGameplayEffectApplied);
}

void UCancerAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

int32 UCancerAbilitySystemComponent::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (!EventTag.MatchesTag(Tag_Combat_Event_Input))
	{
		UE_VLOG(GetOwner(), LogAbilitySystemComponent, Log, TEXT("Event: %s"), *EventTag.ToString());
	}
	return Super::HandleGameplayEvent(EventTag, Payload);
}

void UCancerAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	if (!IsValid(AbilitySpec.Ability))
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("OnGiveAbility Ability ! isValid --[%s]"), *GetOwner()->GetName())
		return;
	}

	if (AllAbilitiesHandleArray.Contains(AbilitySpec.Handle))
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("OnGiveAbility AllAbilitiesHandleMap has [%s] --[%s]"),
		       *AbilitySpec.Ability->GetAssetTags().ToString(), *GetOwner()->GetName());
		return;
	}

	//添加触发标签
	if (auto CancerAbilityCDO = Cast<UCancerGameplayAbility>(AbilitySpec.Ability))
	{
		uint32 TagHash = GetContainerTypeHash(CancerAbilityCDO->GetAssetTags());
		FCancerAbilityDataSet* AbilityData = nullptr;
		if (AbilitiesDefMap.Contains(TagHash))
		{
			AbilityData = AbilitiesDefMap.Find(TagHash);
		}

		if (AbilityData && AbilityData->AbilityDef.TriggerTag.IsValid())
		{
			auto EventTag = AbilityData->AbilityDef.TriggerTag;
			auto& TriggeredAbilityMap = GameplayEventTriggeredAbilities;
			if (TriggeredAbilityMap.Contains(EventTag))
			{
				TriggeredAbilityMap[EventTag].AddUnique(AbilitySpec.Handle);
				// Fixme: is this right? Do we want to trigger the ability directly of the spec?
			}
			else
			{
				TArray<FGameplayAbilitySpecHandle> Triggers;
				Triggers.Add(AbilitySpec.Handle);
				TriggeredAbilityMap.Add(EventTag, Triggers);
			}
		}
	}
	Super::OnGiveAbility(AbilitySpec);
}

void UCancerAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	//移除触发标签
	if (auto CancerAbilityCDO = Cast<UCancerGameplayAbility>(AbilitySpec.Ability))
	{
		uint32 TagHash = GetContainerTypeHash(CancerAbilityCDO->GetAssetTags());
		FCancerAbilityDataSet* AbilityData = nullptr;
		if (AbilitiesDefMap.Contains(TagHash))
		{
			AbilityData = AbilitiesDefMap.Find(TagHash);
		}

		if (AbilityData && AbilityData->AbilityDef.TriggerTag.IsValid())
		{
			auto EventTag = AbilityData->AbilityDef.TriggerTag;
			auto& TriggeredAbilityMap = GameplayEventTriggeredAbilities;
			if (TriggeredAbilityMap.Contains(EventTag))
			{
				TriggeredAbilityMap[EventTag].Remove(AbilitySpec.Handle);
				// Fixme: is this right? Do we want to trigger the ability directly of the spec?
			}
		}
	}

	Super::OnRemoveAbility(AbilitySpec);
}

UGameplayAbility* UCancerAbilitySystemComponent::CreateNewInstanceOfAbility(FGameplayAbilitySpec& Spec,
                                                                            const UGameplayAbility* Ability)
{
	check(Ability);
	check(Ability->HasAllFlags(RF_ClassDefaultObject));

	AActor* Owner = GetOwner();
	check(Owner);

	UGameplayAbility* AbilityInstance = NewObject<UGameplayAbility>(Owner, Ability->GetClass());

	check(AbilityInstance);

	// Add it to one of our instance lists so that it doesn't GC.
	if (AbilityInstance->GetReplicationPolicy() != EGameplayAbilityReplicationPolicy::ReplicateNo)
	{
		Spec.ReplicatedInstances.Add(AbilityInstance);
		AddReplicatedInstancedAbility(AbilityInstance);
	}
	else
	{
		Spec.NonReplicatedInstances.Add(AbilityInstance);
	}

	if (auto CancerAbility = Cast<UCancerGameplayAbility>(AbilityInstance))
	{
		// Primary lookup by AssetTags hash
		uint32 TagHash = GetContainerTypeHash(CancerAbility->GetAssetTags());
		FCancerAbilityDataSet AbilityData;
		if (AbilitiesDefMap.Contains(TagHash))
		{
			AbilityData = *AbilitiesDefMap.Find(TagHash);
		}

		/*if (!AbilityData)
		{
			for (const auto& Pair : AbilitiesDefMap)
			{
				if (Pair.Value && Pair.Value->Ability == CancerAbility->GetClass())
				{
					AbilityData = Pair.Value;
					break;
				}
			}
		}*/

		CancerAbility->SetAbilityData(AbilityData);

		// Do not mutate AbilityTags (deprecated). Use dynamic spec source tags and owned tags flows instead.
		Spec.GetDynamicSpecSourceTags().AddTag(AbilityData.AbilityDef.InputTag);
	}
	return AbilityInstance;
}

void UCancerAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTagContainer& InputTagContainer)
{
	for (auto InputTag : InputTagContainer.GetGameplayTagArray())
	{
		//输入按下
		if (InputTag.IsValid())
		{
			for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
			{
				if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
				{
					InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				}
			}
		}
	}
}

void UCancerAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTagContainer& InputTagContainer)
{
	for (auto InputTag : InputTagContainer.GetGameplayTagArray())
	{
		//输入松开
		if (InputTag.IsValid())
		{
			for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
			{
				if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
				{
					InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				}
			}
		}
	}
}

void UCancerAbilitySystemComponent::AbilityInputTagTriggered(const FGameplayTagContainer& InputTagContainer)
{
	for (auto InputTag : InputTagContainer.GetGameplayTagArray())
	{
		//输入按下
		if (InputTag.IsValid())
		{
			for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
			{
				if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
				{
					InputTriggeredSpecHandles.AddUnique(AbilitySpec.Handle);
				}
			}
		}
	}
}

void UCancerAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	//阻挡输入
	if (HasMatchingGameplayTag(Tag_Input_BlockAbility))
	{
		ClearAbilityInput();
		return;
	}


	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// 处理本帧中输入被按下的所有能力。
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				if (!AbilitySpec->IsActive())
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
				else
				{
					for (UGameplayAbility* Instance : AbilitySpec->GetAbilityInstances())
					{
						if (UCancerGameplayAbility* CancerAbilityIns = Cast<UCancerGameplayAbility>(Instance))
						{
							CancerAbilityIns->AbilityInputStarted();
						}
					}
				}
				FCancerActiveAbilityInfo Info;
				Info.AbilityTag = AbilitySpec->Ability->GetAssetTags();
				bool ove;
				Info.Priority = GetAbilityPriority(Info.AbilityTag,ove).Priority;
				Info.Color = FColor::White;
				OnInputPressed.Broadcast(Info);
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilitySpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				if (!AbilitySpec->IsActive())
				{
					if (TryActivateAbility(AbilitySpecHandle))
					{
						for (UGameplayAbility* Instance : AbilitySpec->GetAbilityInstances())
						{
							if (UCancerGameplayAbility* CancerAbilityIns = Cast<UCancerGameplayAbility>(Instance))
							{
								CancerAbilityIns->AbilityInputStarted();
							}
						}
					}
					
				}
			}
		}
	}

	TArray<FGameplayAbilitySpecHandle> TriggeredAbilitySpecHandle;

	//传递输入
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputTriggeredSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				if (AbilitySpec->IsActive())
				{
					for (UGameplayAbility* Instance : AbilitySpec->GetAbilityInstances())
					{
						if (UCancerGameplayAbility* CancerAbilityIns = Cast<UCancerGameplayAbility>(Instance))
						{
							CancerAbilityIns->AbilityInputTriggered();
							CancerAbilityIns->bIsTriggered = true;
						}
					}
					TriggeredAbilitySpecHandle.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	//上一帧被触发、本帧不再触发 结束输入(由输入缓存触发的技能会存在这个问题)
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && AbilitySpec.IsActive())
		{
			for (UGameplayAbility* Instance : AbilitySpec.GetAbilityInstances())
			{
				if (UCancerGameplayAbility* CancerAbilityIns = Cast<UCancerGameplayAbility>(Instance))
				{
					if (CancerAbilityIns->bIsTriggered && !TriggeredAbilitySpecHandle.Contains(AbilitySpec.Handle))
					{
						CancerAbilityIns->AbilityInputReleased();
						CancerAbilityIns->bIsTriggered = false;
					}
				}
			}
		}
	}

	
	//
	// 处理此帧中已释放输入的所有技能。
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->IsActive())
			{
				for (UGameplayAbility* Instance : AbilitySpec->GetAbilityInstances())
				{
					if (UCancerGameplayAbility* CancerAbilityIns = Cast<UCancerGameplayAbility>(Instance))
					{
						CancerAbilityIns->AbilityInputReleased();
						CancerAbilityIns->bIsTriggered = false;
					}
				}
			}
		}
	}

	//
	//清除缓存的能力句柄。
	//
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputTriggeredSpecHandles.Reset();
}

void UCancerAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputTriggeredSpecHandles.Reset();
}

void UCancerAbilitySystemComponent::SetAbilityTagPriorityOverride(const FGameplayTagContainer& AbilityTag,
                                                                  int32 Priority)
{
	if (!AbilityTag.IsEmpty())
	{
		RuntimeTagPriorityOverrides.Add(GetContainerTypeHash(AbilityTag), Priority);
	}
}

void UCancerAbilitySystemComponent::ClearAbilityTagPriorityOverride()
{
	RuntimeTagPriorityOverrides.Reset();
}

void UCancerAbilitySystemComponent::ClearAbilityTagPriorityOverrideWithTag(const FGameplayTagContainer& AbilityTag)
{
	RuntimeTagPriorityOverrides.Remove(GetContainerTypeHash(AbilityTag));
}


void UCancerAbilitySystemComponent::SetAbilityLevel(FGameplayAbilitySpecHandle SpecHandle, int32 Level)
{
	if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
	{
		Spec->Level = Level;

		MarkAbilitySpecDirty(*Spec, true);

		//AbilityLevelChanged(SpecHandle, Level);
	}
}

int32 UCancerAbilitySystemComponent::GetAbilityLevel(FGameplayAbilitySpecHandle SpecHandle)
{
	if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
	{
		return Spec->Level;
	}

	return -1;
}

void UCancerAbilitySystemComponent::SetLevel(int32 Level)
{
	check(LevelEffect);
	if (!LevelEffect)
	{
		return;
	}
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(LevelEffect, Level, MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	if (!Spec)
	{
		return;
	}
	ApplyGameplayEffectSpecToSelf(*Spec);
}

float UCancerAbilitySystemComponent::GetLevel()
{
	if (auto Attribute = GetSet<UCancerProgressionSet>())
		return Attribute->GetLevel();
	return 1.f;
}

void UCancerAbilitySystemComponent::SetCombatData(UCancerCombatData* InCombatData)
{
	CombatData = InCombatData;
}

FCancerSoulData UCancerAbilitySystemComponent::GetSoulData() const
{
	if (CombatData)
		return CombatData->SoulData;
	return FCancerSoulData();
}


UAnimMontage* UCancerAbilitySystemComponent::GetAbilityMontage(const FGameplayTag MontageTag) const
{
	{
		if (!ChooserTable || !MontageTag.IsValid())
		{
			UE_LOG(LogAbilitySystemComponent, Error, TEXT("GetAbilityMontage:: 标签:[%s]无效 --[%s]"),
			       *MontageTag.ToString()
			       , *GetOwner()->GetName());
			return nullptr;
		}
		FChooserType ChooserType = FChooserType(FGameplayTagContainer(MontageTag));

		if (auto Montage = UCancerChooserLibrary::GetAnimFromChooser<UAnimMontage>(
			GetOwnerActor(), ChooserType, ChooserTable))
		{
			return Montage;
		}
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("GetAbilityMontage:: 标签:[%s]Montage是无效 --  [%s]"),
		       *MontageTag.ToString(),
		       *GetOwner()->GetName())
		return nullptr;
	}
}

void UCancerAbilitySystemComponent::RefreshAbilityMontageMapFromTables(UChooserTable* InChooserTable)
{
	ChooserTable = InChooserTable;
}

FStaggerAnimationConfig UCancerAbilitySystemComponent::GetHitConfig(EStaggerType Module, int32 AnimationID,
                                                                    bool bExactMatch)
{
	return StaggerAnimationData->FindStaggerAnimation(Module, AnimationID, bExactMatch);
}

void UCancerAbilitySystemComponent::SetStaggerAnimData(UStaggerAnimationDataAsset* InStaggerAnimationData)
{
	StaggerAnimationData = InStaggerAnimationData;
}

void UCancerAbilitySystemComponent::SetSpecialAbility(UCancerSpecialAbility* InSpecialAbility)
{
	SpecialAbility = InSpecialAbility;
}

UCancerSpecialAbility* UCancerAbilitySystemComponent::GetSpecialAbility() const
{
	return SpecialAbility;
}

void UCancerAbilitySystemComponent::SetInputTagByAbilityTag(const FGameplayTagContainer& InTagContainer,
                                                            const FGameplayTag& InputTag)
{
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability->GetAssetTags() == InTagContainer)
		{
			AbilitySpec.GetDynamicSpecSourceTags().Reset();
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(InputTag);
		}
	}
}

void UCancerAbilitySystemComponent::SetSpecialAbilityData(UDataTable* InSpecialAbilityData)
{
	TArray<FCancerComboData*> ComboDatas = GetInfo<FCancerComboData>(InSpecialAbilityData);
	for (auto Data : ComboDatas)
	{
		SpecialAbilityData.Add(Data->ComboTag, Data->ComboData);
	}
}

FWeaponData* UCancerAbilitySystemComponent::GetSpecialAbilityComboData(const FGameplayTag& InComboTag)
{
	return SpecialAbilityData.Find(InComboTag);
}

void UCancerAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	Super::OnTagUpdated(Tag, TagExists);
	UE_VLOG(GetOwner(), LogAbilitySystemComponent, Log, TEXT("Tag %s Changed Count: %d"), *Tag.ToString(),
	        GetTagCount(Tag));
}

void UCancerAbilitySystemComponent::OnGameplayEffectApplied(UAbilitySystemComponent* Source,
                                                            const FGameplayEffectSpec& Spec,
                                                            FActiveGameplayEffectHandle Handle)
{
	if (Spec.Def)
	{
		UE_VLOG(GetOwner(), LogAbilitySystemComponent, Log, TEXT("GE Applied: %s, Instigator: %s, Target: %s"),
		        *Spec.Def->GetName(),
		        *GetNameSafe(Spec.GetContext().GetInstigator()),
		        *GetNameSafe(GetOwner()));
	}
}

TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>>& UCancerAbilitySystemComponent::GetTriggerAbilities()
{
	return GameplayEventTriggeredAbilities;
}

bool UCancerAbilitySystemComponent::HasTag(FGameplayTag InTag, bool bExactMatch)
{
	if (!InTag.IsValid()) return false;

	const FGameplayTagContainer& OwnedTags = GameplayTagCountContainer.GetExplicitGameplayTags();
	if (bExactMatch)
	{
		for (const FGameplayTag& Tag : OwnedTags)
		{
			if (Tag.MatchesTagExact(InTag))
			{
				return true;
			}
		}
		return false;
	}
	for (const FGameplayTag& Tag : OwnedTags)
	{
		if (Tag.MatchesTag(InTag))
		{
			return true;
		}
	}
	return false;
}

bool UCancerAbilitySystemComponent::HasAllTags(const FGameplayTagContainer& InTag)
{
	if (InTag.IsEmpty()) return true;

	const FGameplayTagContainer& OwnedTags = GameplayTagCountContainer.GetExplicitGameplayTags();
	for (const FGameplayTag& Tag : InTag)
	{
		bool bFound = false;
		for (const FGameplayTag& Owned : OwnedTags)
		{
			if (Owned.MatchesTagExact(Tag))
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			return false;
		}
	}
	return true;
}

bool UCancerAbilitySystemComponent::HasAnyTags(const FGameplayTagContainer& InTag, bool bExactMatch)
{
	if (InTag.IsEmpty()) return false;

	const FGameplayTagContainer& OwnedTags = GameplayTagCountContainer.GetExplicitGameplayTags();
	if (bExactMatch)
	{
		for (const FGameplayTag& Tag : InTag)
		{
			for (const FGameplayTag& Owned : OwnedTags)
			{
				if (Owned.MatchesTagExact(Tag))
				{
					return true;
				}
			}
		}
		return false;
	}

	for (const FGameplayTag& Tag : InTag)
	{
		for (const FGameplayTag& Owned : OwnedTags)
		{
			if (Owned.MatchesTag(Tag))
			{
				return true;
			}
		}
	}
	return false;
}


bool UCancerAbilitySystemComponent::SetAbilitiesData(UDataTable* InData)
{
	if (!IsValid(InData))
		return false;
	TArray<FCancerAbilityDataSet*> Datases;
	InData->GetAllRows<FCancerAbilityDataSet>(FString(), Datases);
	for (FCancerAbilityDataSet* Data : Datases)
	{
		if (!Data->Ability)
		{
			UE_LOG(LogAbilitySystemComponent, Error,
			       TEXT("SetAbilitiesData: Found null Ability class in data table %s"), *InData->GetName());
			continue;
		}
		if (!Data->AbilityDef.AbilityTag.IsValid())
		{
			UE_LOG(LogAbilitySystemComponent, Error,
			       TEXT("SetAbilitiesData: Found invalid AbilityTag for ability %s in data table %s"),
			       *Data->Ability->GetName(), *InData->GetName());
			continue;
		}

		AbilitiesDefMap.Add(GetContainerTypeHash(Data->AbilityDef.AbilityTag), *Data);
	}
	return true;
}

FGameplayTagContainer UCancerAbilitySystemComponent::GetAbilityTagByInputTag(const FGameplayTag& InInputTag)
{
	for (auto Pair : AbilitiesDefMap)
	{
		if (Pair.Value.AbilityDef.InputTag == InInputTag)
		{
			return Pair.Value.AbilityDef.AbilityTag;
		}
	}
	return FGameplayTagContainer();
}

FGameplayTag UCancerAbilitySystemComponent::GetInputTagByAbilityTag(const FGameplayTagContainer& InTagContainer)
{
	uint32 TagHash = GetContainerTypeHash(InTagContainer);
	if (AbilitiesDefMap.Contains(TagHash))
		return AbilitiesDefMap[TagHash].AbilityDef.InputTag;
	return FGameplayTag();
}

FCancerAbilityDataSet UCancerAbilitySystemComponent::FindAbilityDataByTags(
	const FGameplayTagContainer& InAbilityTag) const
{
	uint32 TagHash = GetContainerTypeHash(InAbilityTag);
	if (!AbilitiesDefMap.Contains(TagHash))
	{
		return {};
	}
	return *AbilitiesDefMap.Find(TagHash);
}

bool UCancerAbilitySystemComponent::GetAbilityIsBlocked(const FGameplayTagContainer& InAbilityTag)
{
	//TODO::默认没有配置 不被阻挡
	bool Override = false;
	auto AbilityPriority = GetAbilityPriority(InAbilityTag, Override);

	int32 PriorityA = AbilityPriority.Priority;
	ECancerAbilityGroup AbilityGroupA = AbilityPriority.ActivationGroup;

	bool bResult = false;
	bool bHasActive = false;

	int32 BestPriorityB = TNumericLimits<int32>::Max();
	ECancerAbilityGroup BestGroupB = ECancerAbilityGroup::Exclusive_Replaceable;

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.IsActive())
		{
			bool OvrrideB = false;
			auto ActivatePriority = GetAbilityPriority(Spec.Ability->GetAssetTags(), OvrrideB);
			int32 PriorityB = ActivatePriority.Priority;
			ECancerAbilityGroup AbilityGroupB = ActivatePriority.ActivationGroup;

			//独立能力不参与技能阻挡
			if (AbilityGroupB == ECancerAbilityGroup::Independent)
			{
				continue;
			}
			if (!bHasActive || AbilityGroupB > BestGroupB || (AbilityGroupB == BestGroupB && PriorityB <
				BestPriorityB))
			{
				bHasActive = true;
				BestPriorityB = PriorityB;
				BestGroupB = AbilityGroupB;
			}
		}
	}

	if (bHasActive)
	{
		if (BestGroupB > AbilityGroupA)
		{
			bResult = true;
		}
		else if (BestGroupB == AbilityGroupA && BestPriorityB < PriorityA)
		{
			bResult = true;
		}
	}
	return bResult;
}

FCancerAbilityPriority UCancerAbilitySystemComponent::GetAbilityPriority(
	const FGameplayTagContainer& InAbilityTag, bool& Override)
{
	Override = false;
	uint32 TagHash = GetContainerTypeHash(InAbilityTag);
	if (!AbilitiesDefMap.Contains(TagHash))
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("GetAbilityPriority: Ability tag not found"));
		return {};
	}

	FCancerAbilityPriority AbilityPriority;
	FCancerAbilityDataSet AbilityData = *AbilitiesDefMap.Find(TagHash);
	AbilityPriority.Priority = AbilityData.AbilityPriority.Priority;
	AbilityPriority.ActivationGroup = AbilityData.AbilityPriority.ActivationGroup;

	//运行时优先级覆盖
	if (RuntimeTagPriorityOverrides.Contains(TagHash))
	{
		AbilityPriority.Priority = RuntimeTagPriorityOverrides[TagHash];
		Override = true;
	}
	return AbilityPriority;
}

bool UCancerAbilitySystemComponent::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining,
                                                               float& CooldownDuration)
{
	if (CooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		TArray<TPair<float, float>> DurationAndTimeRemaining = GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}
	return false;
}

void UCancerAbilitySystemComponent::SetLevelEffect(const TSubclassOf<UGameplayEffect>& InLevelEffect)
{
	/*check(InLevelEffect);*/
	if (InLevelEffect == nullptr)
		return;
	LevelEffect = InLevelEffect;
}

void UCancerAbilitySystemComponent::SetAttributeSetData(const FComboAttributeSet& InComboAttributeSet)
{
	ComboAttributeSet = InComboAttributeSet;
}

const FComboAttributeSet& UCancerAbilitySystemComponent::GetAttributeSetData()
{
	return ComboAttributeSet;
}

void UCancerAbilitySystemComponent::UpgradeLevel(int32 DeltalLevel)
{
	SetLevel(GetLevel() + DeltalLevel);
}

UE_ENABLE_OPTIMIZATION
