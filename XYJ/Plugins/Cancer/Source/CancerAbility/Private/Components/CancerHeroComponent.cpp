#include "Components/CancerHeroComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerAbilitySystemComponent.h"
#include "CancerCameraComponent.h"
#include "CancerNativeGamePlayTag.h"
#include "GameFramework/CancerMeleeScan.h"
#include "AbilitySystemComponent.h"
#include "CancerAbilityFunctionLibrary.h"
#include "CancerAnimationFunctionLibrary.h"
#include "CancerInventoryFunctionLibrary.h"
#include "CancertGlobalAbilitySystem.h"
#include "UIExtensionSystem.h"
#include "Blueprint/UserWidget.h"
#include "Components/CancerCombatAttributeComponent.h"
#include "Equipment/EquipmentInstance.h"
#include "GameFramework/CancerWidgetEntry.h"
#include "Kismet/GameplayStatics.h"

UE_DISABLE_OPTIMIZATION

UCancerHeroComponent::UCancerHeroComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CombatTarget = nullptr;
}

void UCancerHeroComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCancerHeroComponent::OnUnregister()
{
	if (ExtensionHandle.IsValid())
	{
		UUIExtensionSubsystem* ExtSubsystem = GetWorld() ? GetWorld()->GetSubsystem<UUIExtensionSubsystem>() : nullptr;
		if (ExtSubsystem)
		{
			ExtSubsystem->UnregisterExtension(ExtensionHandle);
			ExtensionHandle.Rest();
		}
	}
	Super::OnUnregister();
}

void UCancerHeroComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//旋转朝向目标
	//if (HasCombatTarget())
	//RotateToTarget(DeltaTime);

	
	if (auto ASC = GetAbilitySystemComponentFromOwner())
	{
		const bool InCombat = ASC->HasMatchingGameplayTag(ComboTag);
		if (InCombat)
		{
			if (!bWeaponDrawn)
			{
				ExecuteDrawWeapon();
				bWeaponDrawn = true;
				ExitCombatRemainingTime = ExitCombatTime;
			}
			
			if (ASC->HasAnyMatchingGameplayTags(TriggerTags))
			{
				ExitCombatRemainingTime = ExitCombatTime;
			}
			else
			{
				ExitCombatRemainingTime = FMath::Max(0.f, ExitCombatRemainingTime - DeltaTime);
				if (ExitCombatRemainingTime <= 0.f)
				{
					OnTriggerExitCombo();
				}
			}
		}
		else
		{
			bWeaponDrawn = false;
			ExitCombatRemainingTime = 0.f;
		}
	}

	bool IsValid = false;
	if (auto EquipIns = UCancerInventoryFunctionLibrary::
		K2_GetEquipWeaponInstance(GetOwner(),IsValid))
	{
		if (IsValid)
		{
			if (EquipIns->GetCurrentEquipSocket() !=EEquipSocket::Combo)
			{
				GetAbilitySystemComponentFromOwner()->RemoveLooseGameplayTag(ComboTag);
			}
		}
	}
	
	if (!bIsShowBossWidget)
	{
		if (ExtensionHandle.IsValid())
		{
			UUIExtensionSubsystem* ExtSubsystem = GetWorld()
				                                      ? GetWorld()->GetSubsystem<UUIExtensionSubsystem>()
				                                      : nullptr;
			if (ExtSubsystem)
			{
				ExtSubsystem->UnregisterExtension(ExtensionHandle);
				ExtensionHandle.Rest();
			}
		}
		return;
	}

	if (GetWorld() && GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (auto CombatAttributeComponent = UCancerAbilityFunctionLibrary::GetCancerCombatAttributeComponent(GetOwner()))
	{
		bool HasTarget = HasCombatTarget();
		bool IsDead = CombatAttributeComponent->IsDeadOrDying();
		UUIExtensionSubsystem* ExtensionSubsystem = GetWorld()
			                                            ? GetWorld()->GetSubsystem<UUIExtensionSubsystem>()
			                                            : nullptr;
		if (!ExtensionSubsystem)
		{
			return;
		}
		if (!HasTarget || IsDead)
		{
			if (ExtensionHandle.IsValid())
			{
				ExtensionSubsystem->UnregisterExtension(ExtensionHandle);
				ExtensionHandle.Rest();
				UE_LOG(LogTemp, Warning, TEXT("取消注册UI"));
				return;
			}
		}
		if (ExtensionHandle.IsValid())
		{
			return;
		}
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (!PC)
		{
			return;
		}
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
		if (!LocalPlayer)
		{
			return;
		}
		if (!BossWidgetClass)
		{
			return;
		}
		UCancerWidgetEntry* WidgetEntry = NewObject<UCancerWidgetEntry>();
		WidgetEntry->WidgetClass = BossWidgetClass;
		WidgetEntry->OwnerActor = GetOwner();
		ExtensionHandle = ExtensionSubsystem->RegisterExtensionAsData(SlotID, LocalPlayer, WidgetEntry, -1);
		UE_LOG(LogTemp, Warning, TEXT("注册UI"));
	}
	
	
}

void UCancerHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto It = MeleeScanTimerHandles.CreateIterator(); It; ++It)
	{
		FTimerHandle TimerHandle = It.Value();
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		It.RemoveCurrent();
	}
	if (ExtensionHandle.IsValid())
	{
		UUIExtensionSubsystem* ExtSubsystem = GetWorld() ? GetWorld()->GetSubsystem<UUIExtensionSubsystem>() : nullptr;
		if (ExtSubsystem)
		{
			ExtSubsystem->UnregisterExtension(ExtensionHandle);
			ExtensionHandle.Rest();
		}
	}
	Super::EndPlay(EndPlayReason);
}

void UCancerHeroComponent::InitializeWithAbilitySystem_Implementation()
{
	if (auto ASC = GetAbilitySystemComponentFromOwner())
	{
		for (auto Tag : TriggerTags.GetGameplayTagArray())
		{
			FOnGameplayEffectTagCountChanged& CountChangedEvent = ASC->RegisterGameplayTagEvent(Tag);

			CountChangedEvent.AddUObject(this, &UCancerHeroComponent::OnHandleTrigger);
		}

		for (auto Tag : ExitTags.GetGameplayTagArray())
		{
			FOnGameplayEffectTagCountChanged& CountChangedEvent = ASC->RegisterGameplayTagEvent(Tag);

			CountChangedEvent.AddUObject(this, &UCancerHeroComponent::OnHandleExit);
		}
	}
}

UAbilitySystemComponent* UCancerHeroComponent::GetAbilitySystemComponentFromOwner() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
}


#pragma region 连击


void UCancerHeroComponent::IncrementCombo(int32 Step)
{
	ComboIndex = FMath::Max(0, ComboIndex + FMath::Max(1, Step));
}

void UCancerHeroComponent::ResetCombo()
{
	ComboIndex = 0;
	GetWorld()->GetTimerManager().ClearTimer(IncrementBlockTimerHandle);
}

void UCancerHeroComponent::OnHandleExit_Implementation(const FGameplayTag Tag, int32 NewCount)
{
	//如果拥有退出标签 则直接收刀
	auto ASC = UCancerAbilityFunctionLibrary::GetCancerAbilitySystemComponent(GetOwner());
	if (NewCount < 1 || !ASC->HasTag(ComboTag))
		return;
	OnTriggerExitCombo();
}

void UCancerHeroComponent::OnHandleTrigger_Implementation(const FGameplayTag Tag, int32 NewCount)
{
	auto ASC = UCancerAbilityFunctionLibrary::GetCancerAbilitySystemComponent(GetOwner());
	if (NewCount < 1 || ASC->HasTag(ComboTag))
		return;
	OnTriggerEnterCombo();
}

void UCancerHeroComponent::ExecuteDrawWeapon()
{
	auto ASC = GetAbilitySystemComponentFromOwner();
	//触发拔刀
	FGameplayTag EventTag = Tag_Combat_Event_AbilityTrigger_DrawWeapon;
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	ASC->HandleGameplayEvent(EventTag, &Payload);
}

void UCancerHeroComponent::ExecutePutAwayWeapon()
{
	auto ASC = GetAbilitySystemComponentFromOwner();
	//触发收刀
	FGameplayTag EventTag = Tag_Combat_Event_AbilityTrigger_PutAwayWeapon;
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	ASC->HandleGameplayEvent(EventTag, &Payload);
}

void UCancerHeroComponent::OnTriggerExitCombo_Implementation()
{
	//离开战斗
	ExecutePutAwayWeapon();
	GetAbilitySystemComponentFromOwner()->RemoveLooseGameplayTag(ComboTag);
	bWeaponDrawn = false;
	ExitCombatRemainingTime = 0.f;
}

void UCancerHeroComponent::OnTriggerEnterCombo_Implementation()
{
	//进入战斗
	ExecuteDrawWeapon();
	GetAbilitySystemComponentFromOwner()->AddLooseGameplayTag(ComboTag);
	bWeaponDrawn = true;
	ExitCombatRemainingTime = ExitCombatTime;
	
}

bool UCancerHeroComponent::IncrementBlock(int32 Step)
{
	BlockIndex += Step;
	UE_LOG(LogTemp, Warning, TEXT("UCancerHeroComponent BlockIndex : %d"), BlockIndex);

	if (BlockIndex >= MaxBlockIndex)
	{
		ResetBlock();
		UE_LOG(LogTemp, Warning, TEXT("UCancerHeroComponent Success at MaxBlockIndex %d"), MaxBlockIndex);
		return true;
	}

	GetWorld()->GetTimerManager().SetTimer(IncrementBlockTimerHandle, [this]()
	{
		ResetBlock();
	}, 3.f, false);
	return false;
}

void UCancerHeroComponent::ResetBlock()
{
	BlockIndex = 0;
	UE_LOG(LogTemp, Warning, TEXT("UCancerHeroComponent reset block"));
}
#pragma endregion
#pragma region 目标
const AActor* UCancerHeroComponent::GetCombatTarget()
{
	return CombatTarget;
}

void UCancerHeroComponent::SetCombatTarget(AActor* NewCombatTarget)
{
	AActor* OldCombatTarget = CombatTarget;
	CombatTarget = NewCombatTarget;
}

bool UCancerHeroComponent::HasCombatTarget() const
{
	return CombatTarget != nullptr;
}

void UCancerHeroComponent::RotateToTarget(float FrameDeltaTime)
{
	if (!CombatTarget) return;
	if (auto Movement = UCancerAnimationFunctionLibrary::GetCancerMovementComponent(GetOwner()))
	{
		if (Movement->bOrientRotationToMovement)
			return;
	}
	
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	FVector TargetLocation = CombatTarget->GetActorLocation();

	FVector Direction = TargetLocation - OwnerLocation;
	Direction.Z = 0.f;

	if (Direction.IsNearlyZero())
	{
		return;
	}

	FRotator CurrentRotation = GetOwner()->GetActorRotation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, TargetLocation);
	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, FrameDeltaTime, InterpSpeed);
	GetOwner()->SetActorRotation(NewRotation);
}
#pragma endregion
#pragma region 伤害注册
int32 UCancerHeroComponent::RegisterMeleeScanRequest(UCancerMeleeScan* MeleeScan)
{
	int32 RequestId = INDEX_NONE;

	if (!IsValid(MeleeScan) && MeleeScan->HasValidScanData())
	{
		return RequestId;
	}

	UAbilitySystemComponent* AbilityComponent = GetAbilitySystemComponentFromOwner();
	if (!ensure(IsValid(AbilityComponent)))
	{
		return RequestId;
	}

	//发送近战检测开始
	FGameplayEventData Payload;
	Payload.EventTag = Tag_Combat_Event_MeleeScan_Start;
	Payload.Instigator = MeleeScan->GetInstigator();
	Payload.Target = GetOwner();
	Payload.OptionalObject = MeleeScan;
	AbilityComponent->HandleGameplayEvent(Tag_Combat_Event_MeleeScan_Start, &Payload);
	RequestId = FMath::Rand();

	if (RequestId != INDEX_NONE)
	{
		MeleeScanMap.Add(RequestId, MeleeScan);

		const float Duration = MeleeScan->GetDuration();
		if (Duration > 0.f)
		{
			FTimerHandle TimerHandle;
			const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(
				this, &ThisClass::CancelMeleeScanRequest, RequestId);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, Duration, false);

			if (TimerHandle.IsValid())
			{
				MeleeScanTimerHandles.Add(RequestId, TimerHandle);
			}
		}
	}
	if (!MeleeScanMap.IsEmpty() && !AbilityComponent->HasMatchingGameplayTag(Tag_Combat_State_ActiveMeleeScan))
	{
		AbilityComponent->AddLooseGameplayTag(Tag_Combat_State_ActiveMeleeScan);
	}
	return RequestId;
}

void UCancerHeroComponent::CancelMeleeScanRequest(int32 RequestId)
{
	if (RequestId == INDEX_NONE || !MeleeScanMap.Contains(RequestId))
	{
		return;
	}

	UAbilitySystemComponent* AbilityComponent = GetAbilitySystemComponentFromOwner();

	if (!ensure(IsValid(AbilityComponent)))
	{
		return;
	}

	const UCancerMeleeScan* MeleeScan = MeleeScanMap.FindAndRemoveChecked(RequestId);

	//发送近战检测结束
	FGameplayEventData Payload;
	Payload.EventTag = Tag_Combat_Event_MeleeScan_Stop;
	Payload.Instigator = MeleeScan->GetInstigator();
	Payload.Target = GetOwner();
	Payload.OptionalObject = MeleeScan;
	AbilityComponent->HandleGameplayEvent(Tag_Combat_Event_MeleeScan_Stop, &Payload);

	if (MeleeScanTimerHandles.Contains(RequestId))
	{
		FTimerHandle TimerHandle = MeleeScanTimerHandles.FindAndRemoveChecked(RequestId);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}

	if (MeleeScanMap.IsEmpty())
	{
		AbilityComponent->RemoveLooseGameplayTag(Tag_Combat_State_ActiveMeleeScan);
	}
}


#pragma endregion
#pragma region 摄像机

void UCancerHeroComponent::SetAbilityCameraMode(TSubclassOf<UCancerCameraMode> CameraMode,
                                                const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void UCancerHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

TSubclassOf<UCancerCameraMode> UCancerHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}
	return nullptr;
}
#pragma endregion


UE_ENABLE_OPTIMIZATION
