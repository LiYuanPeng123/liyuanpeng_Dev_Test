#include "GameFramework/CancerProjectileActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "CancerAbilityFunctionLibrary.h"
#include "TargetingSystem/TargetingPreset.h"
#include "TargetingSystem/TargetingSubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Components/CancerHeroComponent.h"
#include "Data/CancerProjectileData.h"
#include "GameFramework/CancerDamageType.h"
#include "GameFramework/CancerMeleeScan.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

static TAutoConsoleVariable<bool> ProjectileVar(TEXT("Cancer.Projectile"), false,TEXT(""));

void ACancerProjectileActor::HandleScanTargetsReceived_Implementation(const UCancerMeleeScan* MeleeScan,
                                                                      const FGameplayAbilityTargetDataHandle& Data)
{
	if (IsValid(MeleeScan))
	{
		UCancerDamageType* DamageType = MeleeScan->DamageType;
		if (!IsValid(DamageType))
		{
			return;
		}
		DamageType->DamageParameter.HitInfo.SourceActor = GetOwner();
		DamageType->DamageParameter.HitInfo.CapActor = this;
		AActor* InstigatorActor = GetOwner();
		for (int32 Idx = 0; Idx < Data.Num(); ++Idx)
		{
			const FHitResult* HitResult = Data.Get(Idx)->GetHitResult();
			AActor* Target = HitResult ? HitResult->GetActor() : nullptr;
			if (!Target) continue;
			DamageType->SetHitResult(*HitResult);
			UCancerAbilityFunctionLibrary::ApplyDamage(Target, DamageType->DamageParameter.HitInfo.SourceActor,DamageType);

			ActorsHit.AddUnique(Target);
			if (CachedMaxHits > 0 && ActorsHit.Num() >= CachedMaxHits)
			{
				HandleProjectileExhausted();
				break;
			}
		}
	}
}

ACancerProjectileActor::ACancerProjectileActor(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
	bEnableDebug = false;
	TargetIndex = 0;
	BounceCount = 0;
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bInterpMovement = true;
	ProjectileMovement->bInterpRotation = true;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->Friction = 0.f;
	ProjectileMovement->BounceVelocityStopSimulatingThreshold = 500.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->Bounciness = 1.f;
	ProjectileMovement->HomingAccelerationMagnitude = 100000.f;
	ProjectileMovement->bAutoActivate = false;
	ProjectileMovement->SetIsReplicated(true);
}

void ACancerProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(ProjectileMovement))
	{
		// Bind the delegates to the internal Handlers, so we can always ensure internal state is correct.
		ProjectileMovement->OnProjectileStop.AddUniqueDynamic(this, &ThisClass::OnProjectileStop);
		ProjectileMovement->OnProjectileBounce.AddUniqueDynamic(this, &ThisClass::OnProjectileBounce);
	}
}

void ACancerProjectileActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const FVector CurrentLocation = GetActorLocation();
	if (AccumulatedTravelDistance <= 0.f)
	{
		LastDistanceSampleLocation = CurrentLocation;
	}
	const float FrameTravel = FVector::Distance(LastDistanceSampleLocation, CurrentLocation);
	AccumulatedTravelDistance += FrameTravel;
	LastDistanceSampleLocation = CurrentLocation;
	if (CachedMaxMoveDistance > 0.f && AccumulatedTravelDistance >= CachedMaxMoveDistance)
	{
		HandleProjectileExhausted();
		return;
	}

#pragma region  贝塞尔曲线
	if (bUseBezier && IsValid(ProjectileMovement))
	{
		const bool HasData = ProjectileDataPtr != nullptr;
		const float InitSpeed =
			HasData ? ProjectileDataPtr->ProjectileType.InitSpeed : ProjectileMovement->InitialSpeed;
		const float FinishSpeed = HasData
			                          ? ProjectileDataPtr->ProjectileType.BezierFinishSpeed
			                          : ProjectileMovement->MaxSpeed;
		const float CurrentSpeed = (BezierT < 0.5f) ? InitSpeed : FinishSpeed;
		const float Length = FMath::Max(BezierTotalLength, KINDA_SMALL_NUMBER);
		const float DtT = (CurrentSpeed / Length) * DeltaSeconds;
		const float PrevT = BezierT;
		BezierT = FMath::Clamp(BezierT + DtT, 0.f, 1.f);
		if (!bHasRelockedAfterInflection && bLockAgainAfterInflection && AimingMode == EProjectileAimingMode::Targeting
			&& PrevT < 0.5f && BezierT >= 0.5f)
		{
			bHasRelockedAfterInflection = true;
			bUseBezier = false;
			ExecuteTargeting();
			return;
		}

		const FVector TargetPoint = BezierPoint(BezierT);
		const FVector Delta = TargetPoint - GetActorLocation();
		const FVector NewVelocity = (Delta.IsNearlyZero())
			                            ? FVector::ZeroVector
			                            : (Delta / FMath::Max(DeltaSeconds, KINDA_SMALL_NUMBER));

		ProjectileMovement->Velocity = NewVelocity;
		const FRotator FaceRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPoint);
		SetActorRotation(FaceRot);

		if (BezierT >= 1.f)
		{
			bUseBezier = false;
		}
	}
#pragma endregion

#pragma region  伤害检测
	// 本帧全局唯一 Actor 集合：跨所有扫描实例按 Actor 去重
	TSet<TWeakObjectPtr<AActor>> UniqueActorsThisFrame;

	for (auto It = MeleeScanMap.CreateIterator(); It; ++It)
	{
		UCancerMeleeScan* MeleeScan = It.Value();
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
					Pair.Value -= DeltaSeconds;
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
							UE_LOG(LogTemp, Verbose, TEXT("[ProjectileScan] 跳过对 %s 的重复命中，冷却剩余 %.3fs (HPS=%d)"),
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
						UE_LOG(LogTemp, Verbose, TEXT("[ProjectileScan] 本帧跨实例去重，跳过目标: %s"), *GetNameSafe(HitActor));
					}
					continue; // 本帧已由其他实例命中并准备广播
				}
				UniqueActorsThisFrame.Add(WeakActor);
				UniqueHitsThisFrame.Add(Hit);
			}

			// 仅在存在“本帧唯一”目标时进行广播
			if (UniqueHitsThisFrame.Num() > 0)
			{
				FGameplayAbilityTargetingLocationInfo StartLocation;
				StartLocation.SourceAbility = nullptr;
				StartLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
				StartLocation.SourceActor = GetOwner();

				const FGameplayAbilityTargetDataHandle TargetData = StartLocation.MakeTargetDataHandleFromHitResults(
					nullptr, UniqueHitsThisFrame);
				OnProjectileScan.Broadcast(MeleeScan, TargetData);
			}
		}
		else
		{
			if (MeleeScan && MeleeScan->bEnableDebug)
			{
				UE_LOG(LogTemp, Warning, TEXT("[ProjectileScan] 移除失效的近战扫描实例: %s"), *GetNameSafe(MeleeScan));
			}
			ActorCooldowns.Remove(MeleeScan);
			It.RemoveCurrent();
		}
	}
#pragma endregion
}

TArray<AActor*> ACancerProjectileActor::GetActorsHit_Implementation() const
{
	TArray<AActor*> Result;
	Result.Reserve(ActorsHit.Num());

	for (const TWeakObjectPtr<AActor>& ActorPtr : ActorsHit)
	{
		if (ActorPtr.IsValid() && ActorPtr->IsValidLowLevelFast())
		{
			Result.Add(ActorPtr.Get());
		}
	}

	return Result;
}


float ACancerProjectileActor::GetSpeed_Implementation() const
{
	if (BounceCount > 0)
	{
		return LastBounceVelocity.Size();
	}
	return IsValid(ProjectileMovement) ? ProjectileMovement->InitialSpeed : 0.f;
}

void ACancerProjectileActor::PrepareLaunch_Implementation()
{
	bIsPrepared = true;
	CachedMaxBounces = 0;
	bTargetingExecuteAsync = false;
	TargetingPreset = nullptr;
	AimingMode = EProjectileAimingMode::Forward;
	AccumulatedTravelDistance = 0.f;
	LastDistanceSampleLocation = GetActorLocation();
	CachedMaxMoveDistance = 0.f;
	CachedMaxHits = 0;
	bLockAgainAfterInflection = false;
	bHasRelockedAfterInflection = false;

	if (ProjectileDataPtr)
	{
		TimeToLiveOutsidePool = ProjectileDataPtr->ProjectileType.LifeTime;
		AimingMode = ProjectileDataPtr->ProjectileType.AimingMode;
		TargetingPreset = ProjectileDataPtr->ProjectileType.TargetingPreset;
		CachedMaxBounces = ProjectileDataPtr->ProjectileType.MaxBounces;
		bTargetingExecuteAsync = ProjectileDataPtr->ProjectileType.bExecuteAsync;
		CachedMaxMoveDistance = ProjectileDataPtr->ProjectileType.MaxMoveDis;
		CachedMaxHits = ProjectileDataPtr->ProjectileType.MaxHits;
		bLockAgainAfterInflection = ProjectileDataPtr->ProjectileType.bLockAgain;

		if (IsValid(ProjectileMovement))
		{
			ProjectileMovement->ProjectileGravityScale = ProjectileDataPtr->ProjectileType.Gravity;
			ProjectileMovement->bInterpMovement = true;
			ProjectileMovement->bInterpRotation = true;
			ProjectileMovement->InitialSpeed = ProjectileDataPtr->ProjectileType.InitSpeed;
			ProjectileMovement->MaxSpeed = ProjectileDataPtr->ProjectileType.MaxSpeed;
			ProjectileMovement->Friction = ProjectileDataPtr->ProjectileType.Friction;
		}

		if (auto Mesh = Execute_GetProjectileMesh(this))
		{
			Mesh->SetRelativeRotation(ProjectileDataPtr->ProjectileEffect.MeshRotator);
			Mesh->SetWorldScale3D(ProjectileDataPtr->ProjectileEffect.MeshScale);
		}
	}
	OnProjectileScan.AddDynamic(this, &ThisClass::HandleScanTargetsReceived);
	ActorsHit.Empty();
	SetupIgnoredActors();

	if (IsValid(ProjectileMovement))
	{
		ProjectileMovement->SetUpdatedComponent(GetRootComponent());
		ProjectileMovement->Activate(true);
	}
}

void ACancerProjectileActor::Launch_Implementation()
{
	Execute_PrepareLaunch(this);

	if (AimingMode == EProjectileAimingMode::Targeting && TargetingPreset)
	{
		// If we are aiming and have a targeting preset, then we need to execute the
		// targeting first. This will provide the actor that we will aim at.
		//
		ExecuteTargeting();
	}
	else if (AimingMode == EProjectileAimingMode::Location)
	{
		LaunchToLocation(Execute_GetTargetLocation(this));
	}
	else
	{
		LaunchForward();
	}
}

void ACancerProjectileActor::InitProjectileData_Implementation(
	const UCancerProjectileData* InProjectileData)
{
	ProjectileDataPtr = const_cast<UCancerProjectileData*>(InProjectileData);
}

int32 ACancerProjectileActor::RegisterMeleeScanRequest_Implementation(UCancerMeleeScan* MeleeScan)
{
	int32 RequestId = INDEX_NONE;

	if (!IsValid(MeleeScan) && MeleeScan->HasValidScanData())
	{
		return RequestId;
	}

	RequestId = FMath::Rand();

	if (RequestId != INDEX_NONE)
	{
		MeleeScanMap.Add(RequestId, MeleeScan);
	}
	return RequestId;
}

void ACancerProjectileActor::CancelMeleeScanRequest_Implementation(int32 RequestId)
{
	if (RequestId == INDEX_NONE || !MeleeScanMap.Contains(RequestId))
	{
		return;
	}
	const UCancerMeleeScan* MeleeScan = MeleeScanMap.FindAndRemoveChecked(RequestId);
}

void ACancerProjectileActor::Activate_Implementation()
{
	Super::Activate_Implementation();
}

void ACancerProjectileActor::SetTargetLocation_Implementation(FVector InTargetLocation)
{
	TargetLocation = InTargetLocation;
}


FRotator ACancerProjectileActor::GetInitialRotation_Implementation() const
{
	return GetActorRotation();
}

void ACancerProjectileActor::SetupIgnoredActors_Implementation()
{
	APawn* InstigatorActor = GetInstigator();
	if (!IsValid(InstigatorActor))
	{
		return;
	}

	TArray<AActor*> AttachedActors;
	InstigatorActor->GetAttachedActors(AttachedActors);
	InstigatorActor->MoveIgnoreActorAdd(this);
}

void ACancerProjectileActor::AdjustHitResult_Implementation(const FHitResult& ImpactResult, FHitResult& AdjustedResult)
{
	if (ImpactResult.IsValidBlockingHit() && ImpactResult.GetComponent()->IsA(UMeshComponent::StaticClass()))
	{
		// This impact already contains a valid Mesh Component. Use it as-is.
		AdjustedResult = ImpactResult;
		DrawNewTarget(ImpactResult, FColor::Cyan);
		return;
	}

	const AActor* Target = ImpactResult.GetActor();
	if (!IsValid(Target))
	{
		AdjustedResult = ImpactResult;
		return;
	}

	FCollisionQueryParams TraceParams = FCollisionQueryParams(SCENE_QUERY_STAT(AdjustProjectileHit));
	TraceParams.TraceTag = TEXT("ProjectileHitTrace");
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(GetInstigator());
	TraceParams.AddIgnoredComponent(ImpactResult.GetComponent());

	const FVector Start = ImpactResult.TraceStart;
	const FRotator Direction = UKismetMathLibrary::FindLookAtRotation(Start, Target->GetActorLocation());
	const FVector End = Start + UKismetMathLibrary::GetForwardVector(Direction) * 500.f;

	/*FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, TraceParams))
	{
		AdjustedResult = Hit;
		DrawNewTarget(Hit, FColor::Green);
	}
	else
	{
		AdjustedResult = ImpactResult;
		DrawNewTarget(Hit, FColor::Yellow);
	}

	DrawScanLine(Hit);*/
}

void ACancerProjectileActor::HandleActorHit_Implementation(const FHitResult& ImpactResult)
{
	if (!ImpactResult.HasValidHitObjectHandle())
	{
		return;
	}

	AActor* ActorHit = ImpactResult.GetActor();
	if (ActorHit->Implements<UAbilitySystemInterface>())
	{
		if (HasAuthority() && ImpactEffectHandle.IsValid())
		{
			FGameplayAbilityTargetingLocationInfo TargetInfo;
			TargetInfo.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
			TargetInfo.SourceActor = this;

			const FGameplayAbilityTargetDataHandle TargetData = TargetInfo.MakeTargetDataHandleFromHitResult(
				nullptr, ImpactResult);
			ApplyGameplayEffectToData(TargetData);
		}

		ActorsHit.AddUnique(ActorHit);
		if (CachedMaxHits > 0 && ActorsHit.Num() >= CachedMaxHits)
		{
			HandleProjectileExhausted();
		}
	}
	else
	{
		FGameplayTagContainer SourceTags = FGameplayTagContainer::EmptyContainer;

		const UAbilitySystemComponent* OwnerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
			GetInstigator());
		if (IsValid(OwnerASC))
		{
			OwnerASC->GetOwnedGameplayTags(SourceTags);
		}

		const FGameplayTagContainer TargetTags = FGameplayTagContainer::EmptyContainer;
	}
}

void ACancerProjectileActor::HandleProjectileExhausted_Implementation()
{
	Execute_Deactivate(this);
}

void ACancerProjectileActor::ApplyGameplayEffectToData(const FGameplayAbilityTargetDataHandle& TargetData)
{
}

FGameplayTagContainer ACancerProjectileActor::GetDynamicGameplayTags_Implementation() const
{
	return FGameplayTagContainer::EmptyContainer;
}

TMap<FGameplayTag, float> ACancerProjectileActor::GetAdditionalSetByCallerMagnitudes_Implementation() const
{
	return TMap<FGameplayTag, float>();
}


void ACancerProjectileActor::HandleTargetReceived(const FTargetingRequestHandle& TargetingHandle)
{
	TArray<AActor*> TargetedActors;

	const UTargetingSubsystem* TargetingSubsystem = UTargetingSubsystem::Get(GetWorld());
	check(IsValid(TargetingSubsystem));

	TargetingSubsystem->GetTargetingResultsActors(TargetingHandle, TargetedActors);
	if (TargetedActors.IsEmpty())
	{
		CurrentTarget = nullptr;
		HandleTargetNotFound();
		return;
	}

	CurrentTarget = TargetedActors[0];
	LaunchToTarget(CurrentTarget);
}


void ACancerProjectileActor::OnActivation_Implementation()
{
	Super::OnActivation_Implementation();
	bHasExhausted = false;
	BounceCount = 0;
	LastBounceVelocity = FVector::ZeroVector;
	CurrentTarget = nullptr;
	ActorsHit.Empty();
}

void ACancerProjectileActor::OnDeactivation_Implementation()
{
	Super::OnDeactivation_Implementation();
	OnProjectileScan.RemoveAll(this);
	MeleeScanMap.Empty();
	if (IsValid(ProjectileMovement))
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
	}

	if (IsValid(GetInstigator()))
	{
		GetInstigator()->MoveIgnoreActorRemove(this);
	}
}


TSoftObjectPtr<UNiagaraSystem> ACancerProjectileActor::SelectHitImpactParticles_Implementation(
	const FHitResult& HitResult, const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const
{
	return nullptr;
}

TSoftClassPtr<UCameraShakeBase> ACancerProjectileActor::SelectHitImpactCameraShake_Implementation(
	const FHitResult& HitResult, const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const
{
	return nullptr;
}

void ACancerProjectileActor::ExecuteTargeting()
{
	if (!GetOwner()->IsA(APawn::StaticClass()))
		return;
	if (auto Target = UCancerAbilityFunctionLibrary::GetCancerCombatManagerComponent
		(GetOwner())->GetCombatTarget())
	{
		LaunchToTarget(Target);
	}
	else
	{
		LaunchForward();
	}


	/*if (!IsValid(TargetingPreset))
	{
		HandleTargetNotFound();
		return;
	}*/

	/*UTargetingSubsystem* TargetingSubsystem = UTargetingSubsystem::Get(GetWorld());
	if (!IsValid(TargetingSubsystem))
		return;
	check(IsValid(TargetingSubsystem));

	FTargetingSourceContext SourceContext;
	SourceContext.SourceActor = this;
	SourceContext.SourceLocation = GetActorLocation();
	SourceContext.InstigatorActor = GetInstigator();

	const FTargetingRequestDelegate Delegate = FTargetingRequestDelegate::CreateWeakLambda(
		this, [this](const FTargetingRequestHandle& Handle) { HandleTargetReceived(Handle); });
	const FTargetingRequestHandle TargetingHandle = UTargetingSubsystem::MakeTargetRequestHandle(
		TargetingPreset, SourceContext);

	if (bTargetingExecuteAsync)
	{
		FTargetingAsyncTaskData& AsyncTaskData = FTargetingAsyncTaskData::FindOrAdd(TargetingHandle);
		AsyncTaskData.bReleaseOnCompletion = true;
		TargetingSubsystem->StartAsyncTargetingRequestWithHandle(TargetingHandle, Delegate);
	}
	else
	{
		FTargetingImmediateTaskData& ImmediateTaskData = FTargetingImmediateTaskData::FindOrAdd(TargetingHandle);
		ImmediateTaskData.bReleaseOnCompletion = true;
		TargetingSubsystem->ExecuteTargetingRequestWithHandle(TargetingHandle, Delegate);
	}*/
}

void ACancerProjectileActor::OnProjectileStop(const FHitResult& ImpactResult)
{
	if (ImpactResult.HasValidHitObjectHandle())
	{
		HandleActorHit(ImpactResult);
		HandleProjectileExhausted();
	}
}

void ACancerProjectileActor::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	BounceCount++;
	LastBounceVelocity = ImpactVelocity;

	if (ImpactResult.HasValidHitObjectHandle())
	{
		HandleActorHit(ImpactResult);
	}

	if (CachedMaxBounces > 0 && BounceCount >= CachedMaxBounces)
	{
		// We can't hit anything else.
		HandleProjectileExhausted();
	}
	else if (AimingMode == EProjectileAimingMode::Targeting)
	{
		// Find a new target to launch towards.
		ExecuteTargeting();
	}
	else
	{
		// Keep bouncing aimlessly.
		LaunchForward();
	}
}

void ACancerProjectileActor::PrintMessage(const FString& Message) const
{
#if WITH_EDITOR
	if (bEnableDebug && IsValid(GEngine))
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.f, FColor::Emerald, Message);
	}
#endif
}

void ACancerProjectileActor::DrawScanLine(const FHitResult& NewHit) const
{
#if WITH_EDITOR
	if (bEnableDebug)
	{
		const FColor Color = NewHit.IsValidBlockingHit() ? FColor::Green : FColor::Red;
		::DrawDebugLine(GetWorld(), NewHit.TraceStart, NewHit.TraceEnd, Color, false, 2.f);
	}
#endif
}

void ACancerProjectileActor::DrawNewTarget(const FHitResult& NewHit, const FColor& Color) const
{
#if WITH_EDITOR
	if (bEnableDebug)
	{
		const FVector StartPosition = NewHit.Location;
		DrawDebugSphere(GetWorld(), StartPosition, 24.f, 12.f, Color, false, 2.f);
	}
#endif
}

void ACancerProjectileActor::LaunchForward_Implementation()
{
	if (BounceCount == 0)
	{
		const FRotator InitialRotation = GetInitialRotation();
		const FVector RotationVector = UKismetMathLibrary::GetForwardVector(InitialRotation);
		if (ProjectileDataPtr && ProjectileDataPtr->ProjectileType.VelocityMode == EProjectileVelocityMode::BezierCurve)
		{
			if (IsValid(ProjectileMovement))
			{
				ProjectileMovement->StopMovementImmediately();
			}
			BezierStart = GetActorLocation();
			const FVector DataOffset = ProjectileDataPtr
				                           ? ProjectileDataPtr->ProjectileType.BezierOffset
				                           : FVector::ZeroVector;
			const FVector Offset = InitialRotation.RotateVector(
				bHasBezierOffsetOverride ? BezierOffsetOverride : DataOffset);
			BezierControl = BezierStart + Offset;
			BezierEnd = BezierStart + RotationVector * (ProjectileDataPtr->ProjectileType.InitSpeed *
				TimeToLiveOutsidePool);
			BezierT = 0.f;
			BezierTotalLength = ComputeBezierLength();
			bUseBezier = true;
		}
		else
		{
			const float Speed = Execute_GetSpeed(this);
			LaunchVelocity = Speed * RotationVector;
			if (IsValid(ProjectileMovement))
			{
				ProjectileMovement->Velocity = LaunchVelocity;
			}
		}
	}
}

void ACancerProjectileActor::LaunchToTarget_Implementation(const AActor* Target)
{
	checkf(IsValid(Target), TEXT("LaunchToTarget called without a target."));
	const FVector Location = Target->GetActorLocation();
	LaunchToLocation(Location);
}

void ACancerProjectileActor::LaunchToLocation_Implementation(FVector Location)
{
	if (bHasRelockedAfterInflection)
	{
		if (IsValid(ProjectileMovement))
		{
			ProjectileMovement->StopMovementImmediately();
		}

		const FRotator Direction = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
		SetActorRotation(Direction);

		const float Speed = Execute_GetSpeed(this);
		LaunchVelocity = Speed * UKismetMathLibrary::GetForwardVector(GetActorRotation());
		if (IsValid(ProjectileMovement))
		{
			ProjectileMovement->Velocity = LaunchVelocity;
		}
		return;
	}
	if (ProjectileDataPtr && ProjectileDataPtr->ProjectileType.VelocityMode == EProjectileVelocityMode::BezierCurve)
	{
		if (IsValid(ProjectileMovement))
		{
			ProjectileMovement->StopMovementImmediately();
		}

		const FRotator Direction = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
		SetActorRotation(Direction);

		BezierStart = GetActorLocation();
		const FVector DataOffset = ProjectileDataPtr
			                           ? ProjectileDataPtr->ProjectileType.BezierOffset
			                           : FVector::ZeroVector;
		const FVector Offset = GetActorRotation().RotateVector(
			bHasBezierOffsetOverride ? BezierOffsetOverride : DataOffset);
		BezierControl = BezierStart + Offset;
		BezierEnd = Location;
		BezierT = 0.f;
		BezierTotalLength = ComputeBezierLength();
		bUseBezier = true;
	}
	else
	{
		if (IsValid(ProjectileMovement))
		{
			ProjectileMovement->StopMovementImmediately();
		}

		const FRotator Direction = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
		SetActorRotation(Direction);

		const float Speed = Execute_GetSpeed(this);
		LaunchVelocity = Speed * UKismetMathLibrary::GetForwardVector(GetActorRotation());
		if (IsValid(ProjectileMovement))
		{
			ProjectileMovement->Velocity = LaunchVelocity;
		}
	}
}

void ACancerProjectileActor::HandleTargetNotFound_Implementation()
{
	LaunchForward();
}

TSoftObjectPtr<USoundBase> ACancerProjectileActor::SelectHitImpactSound_Implementation(const FHitResult& HitResult,
	const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const
{
	return nullptr;
}
#pragma region 辅助函数

void ACancerProjectileActor::PlayNiagaraSystem(const UWorld* World, UNiagaraSystem* System, const FVector& Location,
                                               const FRotator& Rotation)
{
	if (IsValid(World) && !World->bIsTearingDown && IsValid(System))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, System, Location, Rotation, FVector::One(),
		                                               true, true, ENCPoolMethod::AutoRelease, true);
	}
}

void ACancerProjectileActor::PlaySound(const UWorld* World, USoundBase* Sound, const FVector& Location,
                                       const FRotator& Rotation)
{
	if (IsValid(World) && !World->bIsTearingDown && IsValid(Sound))
	{
		UGameplayStatics::PlaySoundAtLocation(World, Sound, Location, Rotation);
	}
}

FVector ACancerProjectileActor::BezierPoint(float T) const
{
	const float U = 1.f - T;
	const FVector P = U * U * BezierStart + 2.f * U * T * BezierControl + T * T * BezierEnd;
	return P;
}

float ACancerProjectileActor::ComputeBezierLength() const
{
	const int32 Segments = 20;
	FVector Prev = BezierPoint(0.f);
	float Sum = 0.f;
	for (int32 i = 1; i <= Segments; ++i)
	{
		const float T = static_cast<float>(i) / static_cast<float>(Segments);
		const FVector Cur = BezierPoint(T);
		Sum += FVector::Distance(Prev, Cur);
		Prev = Cur;
	}
	return Sum;
}
#pragma endregion
#include "GameFramework/CancerProjectileActor.h"

void ACancerProjectileActor::SetBezierOffsetOverride(const FVector& InOffset, bool bEnable)
{
	BezierOffsetOverride = InOffset;
	bHasBezierOffsetOverride = bEnable;
}
