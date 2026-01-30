#include "Animation/AnimNotifyState_MeleeScan.h"

#include "CancerAbilityFunctionLibrary.h"
#include "Components/CancerHeroComponent.h"
#include "GameFramework/CancerDamageType.h"
#include "Interfaces/CancerWeaponInterface.h"

TAutoConsoleVariable<bool> DebugMeleeScanVar(
	TEXT("Cancer.MeleeScan"), false,TEXT("DebugMeleeScan.DebugMeleeScan"));

UAnimNotifyState_MeleeScan::UAnimNotifyState_MeleeScan()
{
}

void UAnimNotifyState_MeleeScan::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp->GetWorld()->IsPreviewWorld())
	{
		if (auto OwnerActor = MeshComp->GetOwner())
		{
			for (auto Damage : Damages)
			{
				if (!Damage || Damage->bEnable) continue;
				auto DamageParameter = Damage->DamageParameter;
				TArray<USceneComponent*> Children;
				MeshComp->GetChildrenComponents(true, Children);
				UCancerMeleeScan* MeleeScan = nullptr;
				if (DamageParameter.TranceInfo.CombatAbilitySource == EDamageDetectionSource::Owner)
				{
					MeleeScan = CreateMeleeScanInstance(
						OwnerActor, OwnerActor, Damage,
						MeshComp, TotalDuration);
				}
				else if (DamageParameter.TranceInfo.CombatAbilitySource == EDamageDetectionSource::Weapon)
				{
					for (auto Child : Children)
					{
						for (auto PreMeshName : DamageParameter.TranceInfo.WeaponNames)
						{
							if (Child->GetAttachSocketName() == PreMeshName)
							{
								if (Child->GetOwner()->Implements<UCancerWeaponInterface>())
								{
									auto WeaponMesh = ICancerWeaponInterface::Execute_GetWeaponMesh(Child->GetOwner());
									if (!WeaponMesh)return;
									if (WeaponMesh->IsA(UMeshComponent::StaticClass()))
									{
										UE_LOG(LogTemp, Warning, TEXT("注册伤害"));
										UMeshComponent* WeaponMeshComponent = Cast<UMeshComponent>(WeaponMesh);
										UCancerDamageType* RuntimeDamage = DuplicateObject<UCancerDamageType>(
											Damage, OwnerActor);
										MeleeScan = CreateMeleeScanInstance(
											MeshComp->GetOwner(), MeshComp->GetOwner(), RuntimeDamage,
											WeaponMeshComponent, TotalDuration);
									}
								}
							}
						}
					}
				}
				else
				{
				}

				if (!MeleeScan)continue;
				if (auto CancerCombat =
					UCancerAbilityFunctionLibrary::GetCancerCombatManagerComponent(
						OwnerActor))
				{
					MeleeScan->bEnableDebug = DebugMeleeScanVar.GetValueOnGameThread();
					if (MeleeScan->HasValidScanData())
						CancerCombat->RegisterMeleeScanRequest(MeleeScan);
				}
			}
		}
	}


#if WITH_EDITOR
	if (MeshComp && MeshComp->GetOwner() && MeshComp->GetWorld() && MeshComp->GetWorld()->IsPreviewWorld())
	{
		for (auto Damage : Damages)
		{
			if (!Damage) continue;
			auto DamageParameter = Damage->DamageParameter;
			TArray<USceneComponent*> Children;
			MeshComp->GetChildrenComponents(true, Children);
			if (DamageParameter.TranceInfo.CombatAbilitySource == EDamageDetectionSource::Owner)
			{
				UCancerMeleeScan* MeleeScan = CreateMeleeScanInstance(
					MeshComp->GetOwner(), MeshComp->GetOwner(), Damage,
					MeshComp, TotalDuration);
				MeleeScan->bEnableDebug = Damage->bEnableDebug;
				MeleeScans.Add(MeleeScan);
			}
			else if (DamageParameter.TranceInfo.CombatAbilitySource == EDamageDetectionSource::Weapon)
			{
				for (auto Child : Children)
				{
					if (Child->IsA(UMeshComponent::StaticClass()))
					{
						for (auto PreMeshName : DamageParameter.TranceInfo.WeaponNames)
						{
							if (Child->GetAttachSocketName() == PreMeshName)
							{
								UMeshComponent* PreMeshComponent = Cast<UMeshComponent>(Child);
								UCancerMeleeScan* MeleeScan = CreateMeleeScanInstance(
									MeshComp->GetOwner(), MeshComp->GetOwner(), Damage,
									PreMeshComponent, TotalDuration);
								MeleeScan->bEnableDebug = Damage->bEnableDebug;
								MeleeScans.Add(MeleeScan);
							}
						}
					}
				}
			}
			else
			{
			}
		}
	}
#endif
}

void UAnimNotifyState_MeleeScan::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

#if WITH_EDITOR
	if (MeshComp && MeshComp->GetWorld() && MeshComp->GetWorld()->IsPreviewWorld())
	{
		if (!MeleeScans.IsEmpty())
		{
			TArray<FHitResult> OutHits;
			for (auto MeleeScan : MeleeScans)
			{
				if (MeleeScan)
					MeleeScan->ScanForTargets(OutHits);
			}
		}
	}
#endif
}

void UAnimNotifyState_MeleeScan::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

#if WITH_EDITOR
	if (MeshComp && MeshComp->GetWorld() && MeshComp->GetWorld()->IsPreviewWorld())
	{
		MeleeScans.Empty();
	}
#endif
}

void UAnimNotifyState_MeleeScan::HandleTimeExpired(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::HandleTimeExpired(MeshComp, Animation);
}

UCancerMeleeScan* UAnimNotifyState_MeleeScan::CreateMeleeScanInstance_Implementation(
	AActor* Owner, AActor* Causer, UCancerDamageType* DamageType,
	UMeshComponent* SourceMesh, float Duration) const
{
	auto DamageParameter = DamageType->DamageParameter;
	DamageType->DamageParameter.HitInfo.SourceActor = Owner;
	DamageType->DamageParameter.HitInfo.CapActor = Owner;
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

FString UAnimNotifyState_MeleeScan::GetNotifyName_Implementation() const
{
	return TEXT("近战扫描");
}
