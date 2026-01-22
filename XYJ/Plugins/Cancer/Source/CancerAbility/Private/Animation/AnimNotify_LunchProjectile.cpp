#include "Animation/AnimNotify_LunchProjectile.h"
#include "CancerActorPoolComponent.h"
#include "CancerActorPoolFunctionLibrary.h"
#include "GameFramework/CancerProjectileActor.h"
#include "Interfaces/CombatProjectileInterface.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Data/CancerProjectileData.h"
#include "Engine/StaticMesh.h"

TAutoConsoleVariable<bool> ProjectileScanVar(
	TEXT("Cancer.ProjectileScan"), false,TEXT("ProjectileScan.ProjectileScan"));

UAnimNotify_LunchProjectile::UAnimNotify_LunchProjectile()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(150, 150, 150, 255);
#endif
}

void UAnimNotify_LunchProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
    if (!MeshComp->GetWorld()->IsPreviewWorld())
    {
        for (const FSpawnProjectileType& SpawnInfo : SpawnProjectiles)
        {
            if (SpawnInfo.bDisableActor)
            {
                continue;
            }
            AActor* SpawnerLocal = nullptr;

			//优先从对象池中获取
			if (auto ActorPool = UCancerActorPoolFunctionLibrary::GetCancerActorPoolComponent(MeshComp->GetOwner()))
			{
                if (SpawnInfo.ProjectileActorClass)
                {
                    SpawnerLocal = ActorPool->GetActorFromPool(SpawnInfo.ProjectileActorClass);
                }
            }
        	if (!SpawnerLocal)
        	{
        		if (!SpawnInfo.ProjectileActorClass)
        		{
        			continue;
        		}
        		SpawnerLocal = MeshComp->GetWorld()->SpawnActor(SpawnInfo.ProjectileActorClass);
        		if (!SpawnerLocal)
        		{
        			continue;
        		}
        	}
        	if (!SpawnerLocal)
        	{
        		break;
        	}
        	SpawnerLocal->SetOwner(MeshComp->GetOwner());

            if (!SpawnerLocal->Implements<UCombatProjectileInterface>())
            {
                SpawnerLocal->Destroy();
                continue;
            }
        	
        	
        	FTransform Transform;
            if (MeshComp->DoesSocketExist(SpawnInfo.ProjectileSocketName))
            {
                Transform = MeshComp->GetSocketTransform(SpawnInfo.ProjectileSocketName, RTS_World);
            }
            else
            {
            	Transform = MeshComp->GetOwner()->GetActorTransform();
            }
        	
        	SpawnerLocal->SetActorLocation(Transform.GetLocation() + SpawnInfo.SocketLocationOffset);
        	FRotator Rotation;
        	if (SpawnInfo.UseOwnerRotation)
        	{
        		Rotation = MeshComp->GetOwner()->GetActorRotation();
        	}
        	else
        	{
        		Rotation = FRotator(Transform.GetRotation().Rotator() + SpawnInfo.SocketRotationOffset);
        	}
        	SpawnerLocal->SetActorRotation(Rotation);
        	
            if (auto ProjectileData = SpawnInfo.ProjectileData.LoadSynchronous())
            {
                UCancerProjectileData* RuntimeData = DuplicateObject<UCancerProjectileData>(ProjectileData, SpawnerLocal);
                if (ACancerProjectileActor* ProjectileActor = Cast<ACancerProjectileActor>(SpawnerLocal))
                {
                    if (SpawnInfo.bOverrideBezierOffset)
                    {
                        RuntimeData->ProjectileType.BezierOffset = SpawnInfo.BezierOffset;
                    }
                    if (SpawnInfo.bOverrideMovementParams)
                    {
                        RuntimeData->ProjectileType.InitSpeed = SpawnInfo.InitSpeed;
                        RuntimeData->ProjectileType.MaxSpeed = SpawnInfo.MaxSpeed;
                        RuntimeData->ProjectileType.Friction = SpawnInfo.Friction;
                        RuntimeData->ProjectileType.Gravity = SpawnInfo.Gravity;
                    }
                    if (SpawnInfo.bOverrideMeshEffect)
                    {
                        RuntimeData->ProjectileEffect.MeshRotator = SpawnInfo.MeshRotator;
                        RuntimeData->ProjectileEffect.MeshScale = SpawnInfo.MeshScale;
                    }
                }
                ICombatProjectileInterface::Execute_InitProjectileData(SpawnerLocal, RuntimeData);
            }

            ICombatProjectileInterface::Execute_Launch(SpawnerLocal);
            if (SpawnerLocal->Implements<UPoolableActorInterface>())
            {
                IPoolableActorInterface::Execute_Activate(SpawnerLocal);
            }

            for (auto Damage : SpawnInfo.Damages)
            {
                if (!Damage) continue;
                auto DamageParameter = Damage->DamageParameter;
                if (DamageParameter.TranceInfo.CombatAbilitySource == EDamageDetectionSource::Projectile)
                {
                    auto Mesh = ICombatProjectileInterface::Execute_GetProjectileMesh(SpawnerLocal);
                    UCancerMeleeScan* MeleeScan = CreateMeleeScanInstance(
                        MeshComp->GetOwner(), MeshComp->GetOwner(), Damage,
                        Mesh, 1);

                    MeleeScan->bEnableDebug = ProjectileScanVar.GetValueOnGameThread();
                    if (MeleeScan->HasValidScanData())
                        ICombatProjectileInterface::Execute_RegisterMeleeScanRequest(SpawnerLocal, MeleeScan);
                }
            }
        }
    }

#if WITH_EDITOR
	if (MeshComp && MeshComp->GetOwner() && MeshComp->GetWorld() && MeshComp->GetWorld()->IsPreviewWorld())
	{
        StopPreviewScanTimer();
        MeleeScans.Empty();
        for (AActor* Old : Spawners)
        {
            if (Old)
            {
                Old->Destroy();
            }
        }
        Spawners.Empty();

		const FVector DefaultPos = MeshComp->GetSocketLocation("root") + TargetOffset;

		if (PreviewSphere)
			PreviewSphere->DestroyComponent();
		if (UStaticMeshComponent* Sphere = NewObject<UStaticMeshComponent>(MeshComp->GetOwner()))
		{
			UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
			Sphere->SetStaticMesh(SphereMesh);
			Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Sphere->SetGenerateOverlapEvents(false);
			Sphere->SetMobility(EComponentMobility::Movable);
			Sphere->RegisterComponent();
			Sphere->SetWorldScale3D(FVector(0.4f));
			Sphere->SetHiddenInGame(false);
			Sphere->SetVisibility(true, true);
			PreviewSphere = Sphere;
			Sphere->SetWorldLocation(DefaultPos);
		}

        for (const FSpawnProjectileType& SpawnInfo : SpawnProjectiles)
        {
            if (SpawnInfo.bDisableActor || SpawnInfo.bEnable)
            {
                continue;
            }
            if (!IsValid(SpawnInfo.ProjectileActorClass))
            {
                continue;
            }
            AActor* SpawnerLocal = MeshComp->GetWorld()->SpawnActor(SpawnInfo.ProjectileActorClass);
            if (!SpawnerLocal)
            {
                continue;
            }
            SpawnerLocal->SetOwner(MeshComp->GetOwner());
            if (!SpawnerLocal->Implements<UCombatProjectileInterface>())
            {
                SpawnerLocal->Destroy();
                continue;
            }
            if (MeshComp->DoesSocketExist(SpawnInfo.ProjectileSocketName))
            {
                FTransform Transform = MeshComp->GetSocketTransform(SpawnInfo.ProjectileSocketName, RTS_World);
                SpawnerLocal->SetActorLocation(Transform.GetLocation() + SpawnInfo.SocketLocationOffset);
                FRotator Rotation = FRotator(Transform.GetRotation().Rotator() + SpawnInfo.SocketRotationOffset);
                SpawnerLocal->SetActorRotation(Rotation);
            }

            if (SpawnerLocal && PreviewSphere)
            {
                const FVector NewLocation = PreviewSphere->GetComponentLocation();
                ICombatProjectileInterface::Execute_SetTargetLocation(SpawnerLocal, NewLocation);
            }
        	float Duration = 0.f;
            if (auto ProjectileData = SpawnInfo.ProjectileData.LoadSynchronous())
            {
                UCancerProjectileData* RuntimeData = DuplicateObject<UCancerProjectileData>(ProjectileData, SpawnerLocal);
            	Duration = RuntimeData->ProjectileType.LifeTime;
                if (ACancerProjectileActor* ProjectileActor = Cast<ACancerProjectileActor>(SpawnerLocal))
                {
                    if (RuntimeData->ProjectileType.AimingMode != EProjectileAimingMode::Forward)
                    {
                        RuntimeData->ProjectileType.AimingMode = EProjectileAimingMode::Location;
                    }
                    if (SpawnInfo.bOverrideBezierOffset)
                    {
                        RuntimeData->ProjectileType.BezierOffset = SpawnInfo.BezierOffset;
                    }
                    if (SpawnInfo.bOverrideMovementParams)
                    {
                        RuntimeData->ProjectileType.InitSpeed = SpawnInfo.InitSpeed;
                        RuntimeData->ProjectileType.MaxSpeed = SpawnInfo.MaxSpeed;
                        RuntimeData->ProjectileType.Friction = SpawnInfo.Friction;
                        RuntimeData->ProjectileType.Gravity = SpawnInfo.Gravity;
                    }
                    if (SpawnInfo.bOverrideMeshEffect)
                    {
                        RuntimeData->ProjectileEffect.MeshRotator = SpawnInfo.MeshRotator;
                        RuntimeData->ProjectileEffect.MeshScale = SpawnInfo.MeshScale;
                    }
                }
                ICombatProjectileInterface::Execute_InitProjectileData(SpawnerLocal, RuntimeData);
            }

            ICombatProjectileInterface::Execute_Launch(SpawnerLocal);
            Spawners.Add(SpawnerLocal);

            for (auto Damage : SpawnInfo.Damages)
            {
                if (!Damage) continue;
                auto DamageParameter = Damage->DamageParameter;
                if (DamageParameter.TranceInfo.CombatAbilitySource == EDamageDetectionSource::Projectile)
                {
                    auto Mesh = ICombatProjectileInterface::Execute_GetProjectileMesh(SpawnerLocal);
                    UCancerMeleeScan* MeleeScan = CreateMeleeScanInstance(
                        MeshComp->GetOwner(), MeshComp->GetOwner(), Damage,
                        Mesh,Duration );
                    MeleeScan->bEnableDebug = Damage->bEnableDebug;
                    MeleeScans.Add(MeleeScan);
                }
            }
        }
    }

	// 启动预览扫描定时器：按设定间隔在预览世界中调用 ScanForTargets
	StartPreviewScanTimer(MeshComp);
#endif
}

UCancerMeleeScan* UAnimNotify_LunchProjectile::CreateMeleeScanInstance_Implementation(AActor* Owner, AActor* Causer,
	UCancerDamageType* DamageType, UMeshComponent* SourceMesh, float Duration) const
{
	auto DamageParameter = DamageType->DamageParameter;
	DamageType->DamageParameter.HitInfo.SourceActor = Owner;
	TSubclassOf<UCancerMeleeScan> ScanClass = DamageParameter.TranceInfo.MeleeScanClass;
	if (!IsValid(ScanClass))
	{
		//使用默认的
		ScanClass = UCancerMeleeScan::StaticClass();
	}

	FVector ShapeDimensions = FVector::ZeroVector;
	switch (DamageParameter.TranceInfo.ScanMode)
	{
	case EMeleeScanMode::LineTrace:
		ShapeDimensions = FVector::ZeroVector;
		break;
	case EMeleeScanMode::BoxSweep:
		ShapeDimensions = DamageParameter.TranceInfo.BoxHalfExtent;
		break;
	case EMeleeScanMode::CapsuleSweep:
		ShapeDimensions = DamageParameter.TranceInfo.CapsuleExtent;
		break;
	case EMeleeScanMode::SphereSweep:
		ShapeDimensions.X = DamageParameter.TranceInfo.SphereRadius;
		break;
	default:
		break;
	}
	UCancerMeleeScan* NewMeleeScan = UCancerMeleeScan::NewInstance(
		ScanClass, Owner, Causer, SourceMesh,
		DamageParameter.TranceInfo.SocketNames,
		DamageParameter.TranceInfo.TraceSocketOffset,
		DamageParameter.TranceInfo.ScanChannels,
		DamageParameter.TranceInfo.ScanMode, ShapeDimensions,
		DamageParameter.TranceInfo.TraceRadius, Duration);

	NewMeleeScan->DamageType = DamageType;
	// Allow repeated hits on the same target according to DamageType settings
	NewMeleeScan->bAllowMultipleHitsOnSameTarget = DamageParameter.TranceInfo.bAllowMultipleHits;
	return NewMeleeScan;
}

FString UAnimNotify_LunchProjectile::GetNotifyName_Implementation() const
{
	return TEXT("发射物");
}

#if WITH_EDITOR
void UAnimNotify_LunchProjectile::StartPreviewScanTimer(USkeletalMeshComponent* MeshComp)
{
	UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!World || !World->IsPreviewWorld() || bPreviewTimerStarted)
	{
		return;
	}

	PreviewMeshComp = MeshComp;
	PreviewElapsed = 0.0f;

#if WITH_EDITORONLY_DATA
	const float Interval = FMath::Max(PreviewScanInterval, 0.01f);
#else
    const float Interval = 0.1f;
#endif

	World->GetTimerManager().SetTimer(
		PreviewScanTimerHandle,
		this,
		&UAnimNotify_LunchProjectile::PreviewScanTick,
		Interval,
		true
	);
	bPreviewTimerStarted = true;
}

void UAnimNotify_LunchProjectile::StopPreviewScanTimer()
{
	UWorld* World = PreviewMeshComp.IsValid() ? PreviewMeshComp->GetWorld() : nullptr;
	if (World)
	{
		World->GetTimerManager().ClearTimer(PreviewScanTimerHandle);
	}

	bPreviewTimerStarted = false;
	PreviewElapsed = 0.0f;
	PreviewMeshComp.Reset();
	if (PreviewSphere)
		PreviewSphere->DestroyComponent();
	PreviewSphere = nullptr;
	// 清理预览扫描实例
	MeleeScans.Empty();
}

void UAnimNotify_LunchProjectile::PreviewScanTick()
{
	USkeletalMeshComponent* MeshComp = PreviewMeshComp.Get();
	UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!World || !World->IsPreviewWorld())
	{
		StopPreviewScanTimer();
		return;
	}

	if (!MeleeScans.IsEmpty())
	{
		TArray<FHitResult> OutHits;
		for (UCancerMeleeScan* MeleeScan : MeleeScans)
		{
			if (MeleeScan)
			{
				MeleeScan->ScanForTargets(OutHits);
			}
		}
	}


#if WITH_EDITORONLY_DATA
	// 预览持续时长到了则停止扫描
	PreviewElapsed += FMath::Max(PreviewScanInterval, 0.01f);
	if (PreviewElapsed >= PreviewScanDuration)
	{
		StopPreviewScanTimer();
	}
#endif
}
#endif
