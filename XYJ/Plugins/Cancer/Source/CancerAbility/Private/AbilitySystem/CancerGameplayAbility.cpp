#include "AbilitySystem/CancerGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "CancerAbilityFunctionLibrary.h"
#include "CancerAbilitySystemComponent.h"
#include "CancerAnimationFunctionLibrary.h"
#include "CancerInputValuePayload.h"
#include "CancerLockingFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/Cost/CancerAbilityCost.h"
#include "AbilitySystem/Tasks/AbilityTask_ScanMeleeTarget.h"
#include "Components/CancerHeroComponent.h"
#include "Components/CancerMotionWarpingComponent.h"
#include "Components/CancerCosmeticsComponent.h"
#include "Interfaces/CancerCoreSystemInterface.h"
#include "Components/CancerCombatAttributeComponent.h"
#include "EngineCamerasSubsystem.h"
#include "Types/FCancerCombatAttackSupport.h"


UCancerGameplayAbility::UCancerGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	ActivationPolicy = ECancerAbilityActivationPolicy::None;
	ActiveCameraMode = nullptr;
}


class AActor* UCancerGameplayAbility::K2_GetOwnerActor(bool& bIsValid, TSubclassOf<AActor> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetActor())
	{
		if (!OutputClass)
		{
			OutputClass = AActor::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

class AController* UCancerGameplayAbility::K2_GetOwnerController(bool& bIsValid, TSubclassOf<AController> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetController())
	{
		if (!OutputClass)
		{
			OutputClass = AController::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

bool UCancerGameplayAbility::IsPawn() const
{
	return GetAvatarActorFromActorInfo() != nullptr;
}

bool UCancerGameplayAbility::IsAIControlled() const
{
	if (APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
	{
		if (AAIController* AIController = Cast<AAIController>(Pawn->GetController()))
		{
			return true;
		}
	}
	return false;
}

UCancerHeroComponent* UCancerGameplayAbility::GetHeroComponentFromActorInfo() const
{
	return (CurrentActorInfo ? UCancerHeroComponent::FindHeroComponent(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

UAnimInstance* UCancerGameplayAbility::GetAnimInstanceFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		if (CurrentActorInfo->AvatarActor.Get()->Implements<UCancerCoreSystemInterface>())
		{
			return ICancerCoreSystemInterface::Execute_GetCombatAnimInstance(CurrentActorInfo->AvatarActor.Get());
		}
	}
	return nullptr;
}

UCancerMotionWarpingComponent* UCancerGameplayAbility::GetMotionWarpingComponentFromActorInfo() const
{
	if (CurrentActorInfo->AvatarActor.Get())
	{
		return UCancerAbilityFunctionLibrary::GetCancerMotionWarpingComponent(CurrentActorInfo->AvatarActor.Get());
	}
	return nullptr;
}

UCancerCosmeticsComponent* UCancerGameplayAbility::GetCosmeticsComponentFromActorInfo() const
{
	if (CurrentActorInfo->AvatarActor.Get())
	{
		return UCancerAbilityFunctionLibrary::GetCancerCosmeticsComponent(CurrentActorInfo->AvatarActor.Get());
	}
	return nullptr;
}

UCancerMovementComponent* UCancerGameplayAbility::GetCancerMovementFromActorInfo() const
{
	if (CurrentActorInfo->AvatarActor.Get())
	{
		return UCancerAnimationFunctionLibrary::GetCancerMovementComponent(CurrentActorInfo->AvatarActor.Get());
	}
	return nullptr;
}

UCancerLockingTargetComponent* UCancerGameplayAbility::GetLockingTargetComponentFromActorInfo() const
{
	if (CurrentActorInfo->AvatarActor.Get())
	{
		return UCancerLockingFunctionLibrary::GetLockingTargetComponent(CurrentActorInfo->AvatarActor.Get());
	}
	return nullptr;
}


bool UCancerGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayTagContainer* SourceTags,
                                                const FGameplayTagContainer* TargetTags,
                                                FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("技能激活失败{%s}"), *GetAssetTags().ToString())
		return false;
	}

	if (auto ASC = GetCancerAbilitySystemComponentFromActorInfo())
	{
		FGameplayTagContainer Tags = GetAssetTags();
		if (ASC->GetAbilityIsBlocked(Tags))
		{
			UE_LOG(LogAbilitySystemComponent, Warning, TEXT("技能被阻挡{%s}"), *GetAssetTags().ToString())
			return false;
		}
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UCancerGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		K2_CancelAbility();
		return;
	}
	InputIndex = 0;
	if (bActivateWaitEvent)
	{
		WaitEventTask = InitializeEventTask(Tag_Combat_Event, false);
		if (WaitEventTask)
		{
			WaitEventTask->ReadyForActivation();
		}
	}
	if (bExecuteCameraAnimWhenActivated && !IsAIControlled())
	{
		ExecuteCameraAnim();
	}
	if (bEnableDebug && GEngine)
	{
		FString DebugStr = TEXT("激活技能：") + GetAssetTags().ToString();
		GEngine->AddOnScreenDebugMessage(1, EnableDebugTime, FColor::Green, DebugStr);
	}
}

void UCancerGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                        bool bReplicateEndAbility, bool bWasCancelled)
{
	FinishLatentTask(WaitEventTask);
	FinishLatentTask(MeleeScanTask);
	InputIndex = 0;
	if (bEnableDebug && GEngine)
	{
		FString DebugStr = TEXT("结束技能：") + GetAssetTags().ToString();
		GEngine->AddOnScreenDebugMessage(1, EnableDebugTime, FColor::Green, DebugStr);
	}
	if (bMatchWhenAbilityEnd && !IsAIControlled())
	{
		if (auto CameraSubsystem = UEngineCamerasSubsystem::GetEngineCamerasSubsystem(GetWorld()))
		{
			if (auto Controller = GetController<APlayerController>())
				CameraSubsystem->StopAllCameraAnimationsOf(Controller, CameraAnimationSequence, bImmediate);
		}
	}
	if (auto ASC = GetCancerAbilitySystemComponentFromActorInfo())
	{
		ASC->ClearAbilityTagPriorityOverride();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UCancerGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	K2_OnAbilityAdded();
	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UCancerGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilitySpec& Spec)
{
	K2_OnAbilityRemoved();
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UCancerGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,
                                                       const FGameplayAbilitySpec& Spec) const
{
	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == ECancerAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) ||
				(NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (
				NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

bool UCancerGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
	{
		return false;
	}

	// Verify we can afford any additional costs
	for (const TObjectPtr<UCancerAbilityCost>& AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (!AdditionalCost->CheckCost(this, Handle, ActorInfo, /*inout*/ OptionalRelevantTags))
			{
				return false;
			}
		}
	}

	return true;
}

void UCancerGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	check(ActorInfo);

	// Used to determine if the ability actually hit a target (as some costs are only spent on successful attempts)
	auto DetermineIfAbilityHitTarget = [&]()
	{
		if (ActorInfo->IsNetAuthority())
		{
			if (UCancerAbilitySystemComponent* ASC = Cast<UCancerAbilitySystemComponent>(
				ActorInfo->AbilitySystemComponent.Get()))
			{
				FGameplayAbilityTargetDataHandle TargetData;
				for (int32 TargetDataIdx = 0; TargetDataIdx < TargetData.Data.Num(); ++TargetDataIdx)
				{
					if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetData, TargetDataIdx))
					{
						return true;
					}
				}
			}
		}

		return false;
	};

	// Pay any additional costs
	bool bAbilityHitTarget = false;
	bool bHasDeterminedIfAbilityHitTarget = false;
	for (const TObjectPtr<UCancerAbilityCost>& AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (AdditionalCost->ShouldOnlyApplyCostOnHit())
			{
				if (!bHasDeterminedIfAbilityHitTarget)
				{
					bAbilityHitTarget = DetermineIfAbilityHitTarget();
					bHasDeterminedIfAbilityHitTarget = true;
				}

				if (!bAbilityHitTarget)
				{
					continue;
				}
			}

			AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
		}
	}
}

void UCancerGameplayAbility::PostInitProperties()
{
	Super::PostInitProperties();
}


bool UCancerGameplayAbility::CommitAnimationCost()
{
	if (!K2_CanExecuteAnimation())
	{
		UE_LOG(LogAbilitySystemComponent, Warning, TEXT("CommitAnimationCost -- 执行 -- K2_CanExecuteAnimation-- 失败 "));
		return false;
	}
	for (const TObjectPtr<UCancerAbilityCost>& AdditionalCost : AnimationCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (AdditionalCost->ShouldOnlyApplyCostOnHit())
			{
				continue;
			}
			if (AdditionalCost->CheckCost(this, GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), {}))
			{
				AdditionalCost->ApplyCost(this, GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
				                          GetCurrentActivationInfo());
			}
			else
			{
				UE_LOG(LogAbilitySystemComponent, Warning, TEXT("CommitAnimationCost -- 动作消耗 -- 失败 "));
				return false;
			}
		}
	}
	return true;
}

bool UCancerGameplayAbility::K2_CanExecuteAnimation_Implementation()
{
	return true;
}

void UCancerGameplayAbility::SetCameraMode(TSubclassOf<UCancerCameraMode> CameraMode)
{
	if (UCancerHeroComponent* HeroComponent = GetHeroComponentFromActorInfo())
	{
		HeroComponent->SetAbilityCameraMode(CameraMode, CurrentSpecHandle);
		ActiveCameraMode = CameraMode;
	}
}

void UCancerGameplayAbility::ClearCameraMode()
{
	if (ActiveCameraMode)
	{
		if (UCancerHeroComponent* HeroComponent = GetHeroComponentFromActorInfo())
		{
			HeroComponent->ClearAbilityCameraMode(CurrentSpecHandle);
		}

		ActiveCameraMode = nullptr;
	}
}

UAbilityTask_WaitGameplayEvent* UCancerGameplayAbility::InitializeEventTask(FGameplayTag Event, bool bOnlyMatchExact)
{
	UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, Event, nullptr, false, bOnlyMatchExact);
	checkf(IsValid(Task), TEXT("Unexpected invalid Event Task instance."));

	Task->EventReceived.AddDynamic(this, &ThisClass::HandleEventReceived);
	return Task;
}

void UCancerGameplayAbility::FinishLatentTasks(const TArray<UAbilityTask*>& Tasks)
{
	for (TObjectPtr<UAbilityTask> Task : Tasks)
	{
		FinishLatentTask(Task);
	}
}

void UCancerGameplayAbility::FinishLatentTask(UAbilityTask* Task)
{
	if (IsValid(Task))
	{
		Task->EndTask();
	}
}

void UCancerGameplayAbility::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	FCancerCombatAttackSupport::HandleEventReceived(this, Payload);
}

bool UCancerGameplayAbility::HasMatchTag(FGameplayTag Tag) const
{
	if (auto ASC = GetCancerAbilitySystemComponentFromActorInfo())
	{
		return ASC->HasMatchingGameplayTag(Tag);
	}
	return false;
}

bool UCancerGameplayAbility::CanReceivedInput(const FGameplayEventData& Payload) const
{
	const UCancerInputValuePayload* InputValuePayload = Cast<UCancerInputValuePayload>(Payload.OptionalObject2);
	if (!InputValuePayload)
		return false;
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();

	if (!ASC)
		return false;
	FGameplayTag InputTag = ASC->GetInputTagByAbilityTag(GetAssetTags());

	if (!InputValuePayload->InputTagContainer.HasTagExact(InputTag))
		return false;
	return true;
}


void UCancerGameplayAbility::InitializeMeleeScanTask(const UCancerMeleeScan* MeleeScan)
{
	MeleeScanTask = UAbilityTask_ScanMeleeTarget::CreateTask(this);
	checkf(IsValid(MeleeScanTask), TEXT("Unexpected invalid Melee Scan Task instance."));

	MeleeScanTask->Join(MeleeScan);
	MeleeScanTask->OnMeleeTargetsFound.AddDynamic(this, &ThisClass::HandleMeleeScanTargetsReceived);
	MeleeScanTask->ReadyForActivation();
}

UAbilityTask_ScanMeleeTarget* UCancerGameplayAbility::GetMeleeScanTask() const
{
	return MeleeScanTask;
}


void UCancerGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (AbilityData.AbilityDef.CooldownTags.Num() > 0)
	{
		if (UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
				CooldownGE->GetClass(), GetAbilityLevel());
			SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(AbilityData.AbilityDef.CooldownTags);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
			return;
		}
	}

	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, CooldownGE, GetAbilityLevel(Handle, ActorInfo));
	}

	//Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
}

FGameplayTagContainer* UCancerGameplayAbility::GetCooldownTags() const
{
	TempCooldownTags.Reset();
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		TempCooldownTags.AppendTags(*ParentTags);
	}
	if (AbilityData.AbilityDef.CooldownTags.IsValid())
	{
		TempCooldownTags.AppendTags(AbilityData.AbilityDef.CooldownTags);
	}
	return &TempCooldownTags;
}

void UCancerGameplayAbility::AbilityInputStarted_Implementation()
{
	InputIndex++;
	UE_LOG(LogTemp, Warning, TEXT("开始输入[%d]%s"), InputIndex, *GetNameSafe(this))
}

void UCancerGameplayAbility::AbilityInputTriggered_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("持续输入%s"), *GetNameSafe(this))
}

void UCancerGameplayAbility::AbilityInputReleased_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("停止输入%s"), *GetNameSafe(this))
}

FVector UCancerGameplayAbility::GetMovementInput() const
{
	if (auto Movement = GetCancerMovementFromActorInfo())
	{
		return Movement->ConsumeInputVector();
	}
	return FVector::ZeroVector;
}

FName UCancerGameplayAbility::GetLockTargetSectionName(const FVector& InVector) const
{
	if (auto LockTargetComponent = GetLockingTargetComponentFromActorInfo())
	{
		if (LockTargetComponent->IsLockTarget())
		{
			auto LockTargetActor = LockTargetComponent->GetLockTarget();
			check(LockTargetActor);
			FVector A = InVector.GetSafeNormal2D();
			FVector B = (LockTargetActor->GetActorLocation() - GetOwningActorFromActorInfo()->GetActorLocation()).
				GetSafeNormal2D();
			float YawA = FMath::RadiansToDegrees(FMath::Atan2(A.Y, A.X));
			float YawB = FMath::RadiansToDegrees(FMath::Atan2(B.Y, B.X));
			float SignedAngleDeg = FMath::FindDeltaAngleDegrees(YawA, YawB);
			if (FMath::Abs(SignedAngleDeg) > 135)
			{
				return FName("B");
			}
			if (FMath::Abs(SignedAngleDeg) < 60)
			{
				return FName("F");
			}
			return SignedAngleDeg > 0 ? FName("L") : FName("R");
		}
	}
	return FName("B");
}

FVector UCancerGameplayAbility::GetLockTargetDir()
{
	if (auto LockTargetComponent = GetLockingTargetComponentFromActorInfo())
	{
		if (LockTargetComponent->IsLockTarget())
		{
			auto LockTargetActor = LockTargetComponent->GetLockTarget();
			check(LockTargetActor);
			return LockTargetActor->GetActorLocation() - GetOwningActorFromActorInfo()->GetActorLocation().
				GetSafeNormal2D();
		}
	}
	return FVector::ZeroVector;
}

void UCancerGameplayAbility::SetActorRotToTarget()
{
	if (auto LockTargetComponent = GetLockingTargetComponentFromActorInfo())
	{
		if (LockTargetComponent->IsLockTarget())
		{
			auto LockTargetActor = LockTargetComponent->GetLockTarget();
			check(LockTargetActor);
			FVector Dir = (LockTargetActor->GetActorLocation() - GetOwningActorFromActorInfo()->GetActorLocation()).
				GetSafeNormal2D();
			GetOwningActorFromActorInfo()->SetActorRotation(Dir.Rotation());
		}
	}
}

void UCancerGameplayAbility::PlayCameraAnim(UCameraAnimationSequence* AnimationSequence,
                                            const FCameraAnimationParams& Params) const
{
}

void UCancerGameplayAbility::ExecuteCameraAnim()
{
	PlayCameraAnim(CameraAnimationSequence, CameraParams);
}

void UCancerGameplayAbility::AddTag(FGameplayTag Tag, int Count)
{
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	ASC->AddLooseGameplayTag(Tag, Count);
}

void UCancerGameplayAbility::RemoveTag(FGameplayTag Tag, int Count)
{
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	ASC->RemoveLooseGameplayTag(Tag, Count);
}

void UCancerGameplayAbility::SetTag(FGameplayTag Tag, int Count)
{
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	ASC->SetLooseGameplayTagCount(Tag, Count);
}


void UCancerGameplayAbility::HandleMeleeScanTargetsReceived_Implementation(const UCancerMeleeScan* MeleeScan,
                                                                           const FGameplayAbilityTargetDataHandle& Data)
{
	if (IsValid(MeleeScan))
	{
		// Pay any additional costs
		for (const TObjectPtr<UCancerAbilityCost>& AdditionalCost : AdditionalCosts)
		{
			if (AdditionalCost != nullptr)
			{
				if (AdditionalCost->ShouldOnlyApplyCostOnHit())
				{
					if (AdditionalCost->CheckCost(this, GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), {}))
					{
						AdditionalCost->ApplyCost(this, GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
						                          GetCurrentActivationInfo());
					}
				}
			}
		}

		// Pay any additional costs
		for (const TObjectPtr<UCancerAbilityCost>& AdditionalCost : AnimationCosts)
		{
			if (AdditionalCost != nullptr)
			{
				if (AdditionalCost->ShouldOnlyApplyCostOnHit())
				{
					if (AdditionalCost->CheckCost(this, GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), {}))
					{
						AdditionalCost->ApplyCost(this, GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
						                          GetCurrentActivationInfo());
					}
				}
			}
		}

		UCancerDamageType* DamageType = MeleeScan->DamageType;
		if (!IsValid(DamageType))
		{
			return;
		}

		AActor* InstigatorActor = GetAvatarActorFromActorInfo();
		for (int32 Idx = 0; Idx < Data.Num(); ++Idx)
		{
			const FHitResult* HitResult = Data.Get(Idx)->GetHitResult();
			AActor* Target = HitResult ? HitResult->GetActor() : nullptr;
			if (!Target) continue;
			DamageType->SetHitResult(*HitResult);
			UCancerAbilityFunctionLibrary::ApplyDamage(Target, DamageType->DamageParameter.HitInfo.SourceActor,
			                                           DamageType);
		}
	}
}

UCancerAbilitySystemComponent* UCancerGameplayAbility::GetCancerAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo
		        ? Cast<UCancerAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get())
		        : nullptr);
}

UCancerCombatAttributeComponent* UCancerGameplayAbility::GetCancerCombatAttributeComponentFromActorInfo() const
{
	if (CurrentActorInfo->AvatarActor.Get())
	{
		return UCancerAbilityFunctionLibrary::GetCancerCombatAttributeComponent(CurrentActorInfo->AvatarActor.Get());
	}
	return nullptr;
}

void UCancerGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
                                         const FGameplayEventData* TriggerEventData)
{
	if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponentFromActorInfo())
	{
		AbilityData = ASC->FindAbilityDataByTags(GetAssetTags());
		if (AbilityData.AbilityDef.AbilityTag.IsValid() && AbilityTags.IsEmpty())
		{
			AbilityTags = AbilityData.AbilityDef.AbilityTag;
		}
	}

	if (AbilityData.AbilityDef.OwnedTags.IsValid())
	{
		ActivationOwnedTags.AppendTags(AbilityData.AbilityDef.OwnedTags);
	}
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCancerGameplayAbility::SetAbilityData(FCancerAbilityDataSet InAbilityData)
{
	AbilityData = InAbilityData;
}

const FCancerAbilityDataSet& UCancerGameplayAbility::GetAbilityData() const
{
	return AbilityData;
}

void UCancerGameplayAbility::AddTriggerData(TEnumAsByte<EGameplayAbilityTriggerSource::Type> InSource,
                                            FGameplayTag InTriggerTag)
{
	FAbilityTriggerData Data;
	Data.TriggerTag = InTriggerTag;
	Data.TriggerSource = InSource;
	AbilityTriggers.Add(Data);
}
