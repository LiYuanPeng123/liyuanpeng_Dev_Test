#include "Equipment/EquipmentInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerCoreFunctionLibrary.h"
#include "Equipment/EquipmentDefinition.h"

UEquipmentInstance::UEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UWorld* UEquipmentInstance::GetWorld() const
{
	if (APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}
	else
	{
		return nullptr;
	}
}

APawn* UEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* UEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

TArray<AActor*> UEquipmentInstance::GetSpawnedActors() const
{
	TArray<AActor*> Actors;
	for (auto EquipmentInfo : EquipmentInfos)
	{
		if (EquipmentInfo.SpawnActor)
			Actors.Add(EquipmentInfo.SpawnActor);
	}
	return Actors;
}

void UEquipmentInstance::SpawnEquipmentActors(const TArray<FEquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		USceneComponent* AttachTarget = UCancerCoreFunctionLibrary::GetCombatMesh(OwningPawn);
		check(AttachTarget);
		if (!AttachTarget)return;
		for (const FEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity,
			                                                          OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			FEquipmentInfo NewEquipInfo;
			NewEquipInfo.SpawnActor = NewActor;
			NewEquipInfo.AttachSockets = SpawnInfo.AttachSockets;
			EquipmentInfos.Add(NewEquipInfo);
		}
		AttachEquipToSlot(EEquipSocket::Equip);
	}
}

void UEquipmentInstance::AttachEquipToSlot(EEquipSocket InEquipSocket)
{
	if (InEquipSocket == CurrentEquipSocket)
		return;
	if (APawn* OwningPawn = GetPawn())
	{
		USceneComponent* AttachTarget = UCancerCoreFunctionLibrary::GetCombatMesh(OwningPawn);
		check(AttachTarget);
		if (!AttachTarget)return;

		// Tag 武器.插槽.剑.右手 我们的插槽名称 统一为 武器类型 加 位置
		for (auto EquipmentInfo : EquipmentInfos)
		{
			if (!EquipmentInfo.SpawnActor)
				continue;
			if (!EquipmentInfo.AttachSockets.Contains(InEquipSocket))
				continue;

			FName SocketName = EquipmentInfo.AttachSockets[InEquipSocket];

			
			if (!AttachTarget->DoesSocketExist(SocketName))
			{
				UE_LOG(LogTemp, Warning, TEXT("[%s] 没有找到对应插槽"), *SocketName.ToString());
				continue;
			}

			EquipmentInfo.SpawnActor->AttachToComponent(AttachTarget,
			                                            FAttachmentTransformRules::SnapToTargetIncludingScale,
			                                            SocketName);
			
		}
		CurrentEquipSocket = InEquipSocket;
	}
}


void UEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : GetSpawnedActors())
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

void UEquipmentInstance::OnEquipped()
{
	if (APawn* OwningPawn = GetPawn())
	{
		// 目标 ASC（角色）
		/*UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningPawn);
		if (!TargetASC)
		{
			K2_OnEquipped();
			return;
		}

		// 选一个 weaponActor 作为“来源”（示例取第一个）
		AActor* WeaponActor = nullptr;
		for (AActor* Actor : SpawnedActors)
		{
			if (IsValid(Actor))
			{
				WeaponActor = Actor;
				break;
			}
		}
		if (!WeaponActor)
		{
			K2_OnEquipped();
			return;
		}*/

		/*
		// 找/建来源 ASC（优先使用已存在的 UCancerAbilitySystemComponent）
		UCancerAbilitySystemComponent* SourceASC = Cast<UCancerAbilitySystemComponent>(
			UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(WeaponActor));
		if (!SourceASC)
		{
			SourceASC = NewObject<UCancerAbilitySystemComponent>(WeaponActor,
																 UCancerAbilitySystemComponent::StaticClass());
			WeaponActor->AddInstanceComponent(SourceASC);
			SourceASC->RegisterComponent();
			// 作为“来源”即可，Avatar 取 weaponActor 自身最清晰
			SourceASC->InitAbilityActorInfo(WeaponActor, WeaponActor);
		}

		// 确保来源 ASC 拥有 UCancerItemAttributeSet
		// （不存在则新建并注册到 ASC）
		bool bHasItemSet = false;
		for (UAttributeSet* Set : SourceASC->GetSpawnedAttributes())
		{
			if (Cast<UCancerItemAttributeSet>(Set))
			{
				bHasItemSet = true;
				break;
			}
		}
		if (!bHasItemSet)
		{
			UAttributeSet* NewSet = NewObject<UCancerItemAttributeSet>(WeaponActor);
			SourceASC->AddAttributeSetSubobject(NewSet);
		}

		// 从片段读取数值并写入来源属性集
		if (UInventoryItemInstance* ItemInst = Cast<UInventoryItemInstance>(Instigator))
		{
			if (const auto* AttrFrag = ItemInst->FindFragmentByClass<UInventoryFragment_EquipItemAttributes>())
			{
				SourceASC->SetNumericAttributeBase(UCancerItemAttributeSet::GetAttackPowerBonusAttribute(),
												   AttrFrag->AttackPowerBonus);
				SourceASC->SetNumericAttributeBase(UCancerItemAttributeSet::GetDefenseBonusAttribute(),
												   AttrFrag->DefenseBonus);
				SourceASC->SetNumericAttributeBase(UCancerItemAttributeSet::GetDamageBonusPctBonusAttribute(),
												   AttrFrag->DamageBonusPctBonus);
				SourceASC->SetNumericAttributeBase(UCancerItemAttributeSet::GetDamageReductionPctBonusAttribute(),
												   AttrFrag->DamageReductionPctBonus);
				SourceASC->SetNumericAttributeBase(UCancerItemAttributeSet::GetWeaponLevelAttribute(),
												   AttrFrag->WeaponLevel);
			}
		}

		// 应用持久绑定效果：来源=weaponActor 的 ASC；目标=角色 ASC（不快照，实时更新）
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(UCancerEffect_EquipFromSource::StaticClass(),
																		   1.0f, SourceASC->MakeEffectContext());
		if (SpecHandle.IsValid())
		{
			EquipBindingEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		}
	}*/
	}
	// 蓝图回调
	K2_OnEquipped();
}

void UEquipmentInstance::OnUnequipped()
{
	// 移除绑定效果（来源不需要特殊处理，weaponActor 会在 DestroyEquipmentActors 中销毁）
	if (APawn* OwningPawn = GetPawn())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
			OwningPawn))
		{
			if (EquipBindingEffectHandle.IsValid())
			{
				TargetASC->RemoveActiveGameplayEffect(EquipBindingEffectHandle);
				EquipBindingEffectHandle = FActiveGameplayEffectHandle();
			}
		}
	}

	// 蓝图回调
	K2_OnUnequipped();
}
