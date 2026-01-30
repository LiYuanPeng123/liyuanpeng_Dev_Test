#include "AbilitySystem/Abilities/CombatAbility_HitStagger.h"
#include "CancerAbilityFunctionLibrary.h"
#include "CancerAbilitySetting.h"
#include "CancerCoreFunctionLibrary.h"
#include "NiagaraCommon.h"
#include "AbilitySystem/Attributes/CancerCombatSet.h"
#include "Components/CancerCombatAttributeComponent.h"
#include "Components/CancerMotionWarpingComponent.h"
#include "GameFramework/Character.h"


FName GetFirstCharAsFName(const FName& OriginalName)
{
	FString NameString = OriginalName.ToString();

	if (!NameString.IsEmpty())
	{
		// Optimize: Use static buffer to avoid heap allocation for single char
		TCHAR CharBuffer[2] = { NameString[0], TEXT('\0') };
		return FName(CharBuffer);
	}

	return FName();
}

bool MontageHasSection(UAnimMontage* Montage, const FName& SectionName)
{
	if (Montage)
	{
		for (const auto& Sec : Montage->CompositeSections)
		{
			if (Sec.SectionName == SectionName)
			{
				return true;
			}
		}
	}
	
	return false;
};

UCombatAbility_HitStagger::UCombatAbility_HitStagger(const FObjectInitializer& ObjectInitializer)
{
}

void UCombatAbility_HitStagger::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo,
                                                const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	/*if (TriggerEventData)
	{
		HandleEventReceived(*TriggerEventData);
	}*/
}


void UCombatAbility_HitStagger::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);

	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	if (Payload.EventTag == Tag_Combat_Event_LinkHitWindow_Start)
	{
		ASC->AddLooseGameplayTag(Tag_Combat_State_LinkHitWindow);
	}
	else if (Payload.EventTag == Tag_Combat_Event_LinkHitWindow_End)
	{
		ASC->RemoveLooseGameplayTag(Tag_Combat_State_LinkHitWindow);
	}
	else if (Payload.EventTag == Tag_Combat_Event_AbilityTrigger_Stagger)
	{
		FCancerHit HitInfo;
		if (!CanCheckHit(&Payload, HitInfo))
		{
			K2_EndAbility();
			return;
		}
		if (!HitInfo.Montage)
		{
			K2_EndAbility();
			return;
		}
		if (auto MM = UCancerAbilityFunctionLibrary::GetCancerMotionWarpingComponent
			(GetOwningActorFromActorInfo()))
		{
			MM->SetPendingScaleDistance(HitInfo.KnockbackDistance);
		}


		if (!MontageHasSection(HitInfo.Montage, HitInfo.PrimarySection))
		{
			UE_LOG(LogTemp, Warning, TEXT("没有找到Section --[%s][%s]"), *GetNameSafe(HitInfo.Montage)
			       , *HitInfo.PrimarySection.ToString());
			HitInfo.PrimarySection = NAME_None;
		}
		SetupAndPlayAnimation(HitInfo.Montage, HitInfo.PrimarySection);
		UE_LOG(LogTemp, Warning, TEXT("受击硬质 --[%s][%s][%f] "), *GetNameSafe(HitInfo.Montage)
		       , *HitInfo.PrimarySection.ToString(), HitInfo.MontageDuration);

		ScaleHitDuration(HitInfo.Montage, HitInfo.MontageDuration, HitInfo.PrimarySection);

		//受击者
		/*if (OutCameraShake)
		{
			if (auto VictimCharacter = Cast<ACharacter>(Victim))
			{
				if (APlayerController* PC = Cast<APlayerController>(VictimCharacter->GetController()))
				{
					if (PC->IsLocalController())
					{
						PC->ClientStartCameraShake(OutCameraShake);
					}
				}
			}
		}*/
	}
}

void UCombatAbility_HitStagger::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           bool bReplicateEndAbility,
                                           bool bWasCancelled)
{
	if (auto ASC = GetCancerAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(Tag_Combat_State_LinkHitWindow);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UCombatAbility_HitStagger::CanCheckHit(const FGameplayEventData* TriggerEventData, FCancerHit& OutHit)
{
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	if (!ASC) return false;
	if (!TriggerEventData || !TriggerEventData->OptionalObject)
		return false;

	auto DamageType = Cast<UCancerDamageType>(TriggerEventData->OptionalObject);
	if (DamageType == nullptr)
	{
		ensureMsgf(DamageType, TEXT("没有传递有效的DamageType"));
		return false;
	}

	const FCancerHitInfo& DamageInfo = DamageType->DamageParameter.HitInfo;

	float ImpactForce = DamageType->GetHitInfo().ImpactForce;
	int32 StaggerAnimationID = DamageInfo.StaggerAnimationID;
	OutHit.MontageDuration = DamageInfo.HardDuration;
	OutHit.KnockbackDistance = DamageInfo.KnockbackDistance;

	// 如果是完美弹反 将受击ID改为项目设置中的完美弹反动画ID
	ApplyPerfectBlockOverride(DamageType, ImpactForce, StaggerAnimationID, OutHit);

	FName Bone = DamageType->GetHitInfo().HitResult.BoneName;
	
	UE_LOG(LogTemp, Warning, TEXT("击中的骨骼[%s]"), *Bone.ToString());
	
	if (!CanBeHit(ImpactForce))
	{
		
		
		UE_LOG(LogTemp, Warning, TEXT("霸体 --无视受击硬质"));
		return false;
	}
	
	EStaggerType StaggerType = GetStaggerType();
	FStaggerAnimationConfig HitComfig = GetHitAnimData(StaggerType, StaggerAnimationID);
	AActor* Attacker = DamageInfo.SourceActor;
	const FVector AttackerLoc = Attacker ? Attacker->GetActorLocation() : FVector::ZeroVector;
	const FVector ImpactLoc = DamageInfo.HitResult.ImpactPoint;

	//TODO::空中的受击的方向
	// 对于空中受击或浮空连，使用3D方向计算（bPlanar = false）
	bool bPlanar = (StaggerType != EStaggerType::Air && StaggerType != EStaggerType::KnockFly);
	FVector HitDirection = CalculateHitDirection(AttackerLoc, ImpactLoc, bPlanar);
	
	FName PrimarySection = GetDirectionalSectionName(HitDirection, HitComfig.DirectionType);
	
	PrimarySection = FName(PrimarySection.ToString() + TEXT("1"));
	OutHit.Montage = HitComfig.StaggerMontage;
	OutHit.PrimarySection = PrimarySection;
	return true;
}

void UCombatAbility_HitStagger::ApplyPerfectBlockOverride(const UCancerDamageType* DamageType, float& ImpactForce, int32& StaggerAnimationID, FCancerHit& OutHit)
{
	if (DamageType->DamageParameter.DamageFeedback.DamageInfo.HasTag(Tag_Combat_DamageFeedBack_PerfectBlock))
	{
		const UCancerAbilitySetting* Setting = GetDefault<UCancerAbilitySetting>();
		ImpactForce = Setting->PerfectBlockImpactForce;
		StaggerAnimationID = Setting->PerfectBlockStaggerAnimationID;
		OutHit.MontageDuration = Setting->PerfectBlockHardDuration;
		OutHit.KnockbackDistance = Setting->PerfectBlockKnockbackDistance;
	}
}

FName UCombatAbility_HitStagger::GetDirectionalSectionName(const FVector& HitDirection, EStaggerAnimationDirection DirectionType)
{
	ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
	const FVector ForwardVector = Character ? Character->GetActorForwardVector() : FVector::ForwardVector;
	const FVector RightVector = Character ? Character->GetActorRightVector() : FVector::RightVector;
	const float ForwardDot = FVector::DotProduct(HitDirection, ForwardVector);
	const float RightDot = FVector::DotProduct(HitDirection, RightVector);

	if (DirectionType == EStaggerAnimationDirection::FourDirection)
	{
		if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
		{
			return (ForwardDot >= 0.0f) ? FName(TEXT("F")) : FName(TEXT("B"));
		}
		else
		{
			return (RightDot >= 0.0f) ? FName(TEXT("R")) : FName(TEXT("L"));
		}
	}
	else if (DirectionType == EStaggerAnimationDirection::TwoDirection)
	{
		return (ForwardDot >= 0.0f) ? FName(TEXT("F")) : FName(TEXT("B"));
	}
	
	return NAME_None;
}

int32 UCombatAbility_HitStagger::GetIronBodyLevel()
{
	if (const auto ASC = GetAbilitySystemComponentFromActorInfo())
	{
		const float IronBodyValue = ASC->GetNumericAttribute(UCancerCombatSet::GetIronBodyAttribute());
		if (!FMath::IsNearlyZero(IronBodyValue))
		{
			return static_cast<int32>(IronBodyValue);
		}
	}
	if (AActor* Owner = GetOwningActorFromActorInfo())
	{
		if (auto CombatAttr = UCancerAbilityFunctionLibrary::GetCancerCombatAttributeComponent<
			UCancerCombatAttributeComponent>(Owner))
		{
			return static_cast<int32>(CombatAttr->GetIronBody());
		}
	}
	return 0;
}

bool UCombatAbility_HitStagger::CanBeHit(float ImpactForce)
{
	int32 IronBodyLevel = GetIronBodyLevel();
	// 冲击力必须大于霸体等级才能造成硬直
	bool canBeStaggered = ImpactForce > IronBodyLevel;
	return canBeStaggered;
}

FVector UCombatAbility_HitStagger::CalculateHitDirection(const FVector& AttackerLocation,
                                                         const FVector& HitLocation, bool bPlanar) const
{
	auto Ownering = GetOwningActorFromActorInfo();
	FVector OwnerLocation = Ownering->GetActorLocation();

	FVector Direction;
	
	if (bPlanar)
	{
		Direction = (AttackerLocation - OwnerLocation).GetSafeNormal2D();

		if (Direction.IsNearlyZero())
		{
			Direction = (HitLocation - OwnerLocation).GetSafeNormal2D();
		}
	}
	else
	{
		Direction = (AttackerLocation - OwnerLocation).GetSafeNormal();

		if (Direction.IsNearlyZero())
		{
			Direction = (HitLocation - OwnerLocation).GetSafeNormal();
		}
	}

	if (Direction.IsNearlyZero())
	{
		Direction = Ownering->GetActorForwardVector();
	}

	return Direction;
}

void UCombatAbility_HitStagger::ScaleHitDuration(UAnimMontage* Montage, float TargetDuration, FName SelectSection)
{
	if (!Montage || TargetDuration <= 0.0f || !SelectSection.IsValid())
	{
		return;
	}

	// 获取当前播放的片段名称
	// 这里假设动画片段按照命名规则：F1, F2, F3 (前), B1, B2, B3 (后), L1, L2, L3 (左), R1, R2, R3 (右)
	// 我们需要获取片段1和片段2的时长

	TArray<FName> SectionNames;
	for (int32 i = 0; i < Montage->CompositeSections.Num(); ++i)
	{
		SectionNames.Add(Montage->CompositeSections[i].SectionName);
	}
	FName NameSection = GetFirstCharAsFName(SelectSection);
	if (NameSection.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("ScaleHitDuration: Invalid Section Name derived from %s"), *SelectSection.ToString());
		return;
	}

	// 查找前摇段和定身段
	FName PreSection = FName(*(NameSection.ToString() + TEXT("1")));;
	FName HoldSection = FName(*(NameSection.ToString() + TEXT("2")));;

	if (PreSection != NAME_None && HoldSection != NAME_None &&
		MontageHasSection(Montage, PreSection) && MontageHasSection(Montage, HoldSection))
	{
		float PreDuration = GetSectionDuration(Montage, PreSection);
		float HoldDuration = GetSectionDuration(Montage, HoldSection);
		float TotalDuration = PreDuration + HoldDuration;

		if (TotalDuration > 0.0f)
		{
			if (FMath::IsNearlyEqual(TotalDuration, TargetDuration, 0.01f))
			{
				// 时长匹配，不需要调整
				return;
			}
			else if (TotalDuration < TargetDuration)
			{
				// 需要放慢，同时缩放前摇段和定身段
				float ScaleRatio = TotalDuration / TargetDuration;
				SetSectionPlayRate(Montage, PreSection, ScaleRatio);
				SetSectionPlayRate(Montage, HoldSection, ScaleRatio);
			}
			else if (TotalDuration > TargetDuration)
			{
				if (PreDuration >= TargetDuration)
				{
					// 前摇段就超过了目标时长，需要同时加快前摇段和定身段
					float ScaleRatio = TotalDuration / TargetDuration;
					SetSectionPlayRate(Montage, PreSection, ScaleRatio);
					SetSectionPlayRate(Montage, HoldSection, ScaleRatio);
				}
				else
				{
					// 只加快定身段
					float NewHoldDuration = TargetDuration - PreDuration;
					if (NewHoldDuration > 0.0f)
					{
						float HoldScaleRatio = HoldDuration / NewHoldDuration;
						SetSectionPlayRate(Montage, HoldSection, HoldScaleRatio);
					}
				}
			}
		}
	}
}

float UCombatAbility_HitStagger::GetSectionDuration(UAnimMontage* Montage, const FName& SectionName) const
{
	if (!Montage)
	{
		return 0.0f;
	}

	int32 SectionIndex = Montage->GetSectionIndex(SectionName);
	if (SectionIndex == INDEX_NONE)
	{
		return 0.0f;
	}

	const FCompositeSection& Section = Montage->CompositeSections[SectionIndex];

	// 计算片段的结束时间
	float EndTime = 0.0f;
	if (SectionIndex + 1 < Montage->CompositeSections.Num())
	{
		EndTime = Montage->CompositeSections[SectionIndex + 1].GetTime();
	}
	else
	{
		EndTime = Montage->GetPlayLength();
	}

	return EndTime - Section.GetTime();
}

void UCombatAbility_HitStagger::SetSectionPlayRate(UAnimMontage* Montage, const FName& SectionName, float PlayRate)
{
	UAnimInstance* AnimInstance = UCancerCoreFunctionLibrary::GetCombatAnimInstance(
		GetOwningActorFromActorInfo());
	if (!Montage || !AnimInstance)
	{
		return;
	}
	// 设置片段的播放速率
	AnimInstance->Montage_SetPlayRate(Montage, PlayRate);
}

FStaggerAnimationConfig UCombatAbility_HitStagger::GetHitAnimData(EStaggerType Module, int32 AnimationID,
                                                                  bool bExactMatch)
{
	auto CASC = GetCancerAbilitySystemComponentFromActorInfo();
	if (!CASC)
		return FStaggerAnimationConfig();
	return CASC->GetHitConfig(Module, AnimationID, bExactMatch);
}

EStaggerType UCombatAbility_HitStagger::GetStaggerType()
{
	if (auto CASC = GetCancerAbilitySystemComponentFromActorInfo())
	{
		if (CASC->HasTag(Tag_Combat_State_KnockDown))
		{
			return EStaggerType::KnockDown;
		}
		else if (CASC->HasTag(Tag_Combat_State_KnockFly))
		{
			return EStaggerType::KnockFly;
		}
		else if (CASC->HasTag(Movement_Air_Falling) || CASC->HasTag(Movement_Air_Flying))
		{
			return EStaggerType::Air;
		}
		
		
		return EStaggerType::Normal;
	}
	return EStaggerType::Normal;
}
