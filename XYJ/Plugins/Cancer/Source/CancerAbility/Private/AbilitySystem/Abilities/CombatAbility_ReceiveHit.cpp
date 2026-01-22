#include "AbilitySystem/Abilities/CombatAbility_ReceiveHit.h"

#include "AbilitySystemGlobals.h"
#include "CancerAbilityFunctionLibrary.h"
#include "CancerAbilitySystemComponent.h"
#include "CancerNativeGamePlayTag.h"
#include "CancertGlobalAbilitySystem.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/CancerDamageType.h"
#include "GameplayCueFunctionLibrary.h"
#include "AbilitySystem/Attributes/CancerSoulSet.h"
#include "Components/CancerCombatAttributeComponent.h"
#include "Components/CancerHeroComponent.h"
#include "Statics/CancerStaticsSubsystem.h"

UCombatAbility_ReceiveHit::UCombatAbility_ReceiveHit(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCombatAbility_ReceiveHit::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo,
                                                const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bCharacterDead = false;
	bCharacterNoSoul = false;
	if (TriggerEventData)
	{
		HandleEventReceived(*TriggerEventData);
	}
}

void UCombatAbility_ReceiveHit::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCombatAbility_ReceiveHit::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);

	if (Payload.EventTag == Tag_Combat_Event_AbilityTrigger_Death)
	{
		bCharacterDead = true;
		return;
	}

	if (Payload.EventTag == Tag_Combat_Event_AbilityTrigger_Weak)
	{
		bCharacterNoSoul = true;
		return;
	}

	if (Payload.EventTag != Tag_Combat_Event_AbilityTrigger_ReceiveHit)
		return;

	// 取伤害类型
	const UCancerDamageType* DamageType = Cast<UCancerDamageType>(Payload.OptionalObject);
	if (!IsValid(DamageType) || !DamageType->DamageParameter.HitInfo.DamageTag.IsValid())
	{
		K2_EndAbility();
		return;
	}
	// 受击者的 ASC 与当前状态标签
	AActor* TargetActor = GetAvatarActorFromActorInfo();
	UCancerAbilitySystemComponent* TargetASC =
		UCancerAbilityFunctionLibrary::GetCancerAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		K2_EndAbility();
		return;
	}

	const bool bHasInvulnerableWindow = TargetASC->HasMatchingGameplayTag(Tag_Combat_State_InvulnerableWindow);
	const bool bHasPerfectBlockWindow = TargetASC->HasMatchingGameplayTag(Tag_Combat_State_PerfectBlockWindow);
	const bool bHasBlockWindow = TargetASC->HasMatchingGameplayTag(Tag_Combat_State_BlockWindow);

	FCancerHitEffectInfo HitEffectInfo = DamageType->GetHitEffectInfo();
	// 与伤害类型的对比：为 true 表示“会被该帧影响”（即不能无视）
	const bool bAffectedByInvulnerable = bHasInvulnerableWindow && HitEffectInfo.bInvincible;
	const bool bAffectedByPerfectBlock = bHasPerfectBlockWindow && HitEffectInfo.bReflected;
	const bool bAffectedByBlock = bHasBlockWindow && HitEffectInfo.bBlocked;

	//免疫
	if (bAffectedByInvulnerable)
	{
		HandleReceiveInvulnerable(Payload);
		ReceiveInvulnerable.Broadcast(Payload);
		return;
	}
	//弹反
	if (bAffectedByPerfectBlock)
	{
		HandleReceivePerfectBlock(Payload);
		ReceivePerfectBlock.Broadcast(Payload);
		return;
	}
	//格挡
	if (bAffectedByBlock)
	{
		HandleReceiveBlock(Payload);
		ReceiveBlock.Broadcast(Payload);
		return;
	}
	//接收伤害
	HandleReceiveDamage(Payload);
	ReceiveDamage.Broadcast(Payload);

	K2_EndAbility();
}

void UCombatAbility_ReceiveHit::SendDamageFeedback(const UCancerDamageType* DamageInfo,
                                                   const FGameplayTagContainer& FeedBackInfo)
{
	UCancerDamageType* MutableDamageInfo = const_cast<UCancerDamageType*>(DamageInfo);
	MutableDamageInfo->DamageParameter.DamageFeedback.DamageInfo = FeedBackInfo;
	if (bCharacterDead)
	{
		MutableDamageInfo->DamageParameter.DamageFeedback.DamageInfo.AddTag(Tag_Combat_DamageFeedBack_Kill);
	}
	if (bCharacterNoSoul)
	{
		MutableDamageInfo->DamageParameter.DamageFeedback.DamageInfo.AddTag(Tag_Combat_DamageFeedBack_NoSoul);
	}
	FGameplayEventData EventData;
	EventData.EventTag = Tag_Combat_Event_AbilityTrigger_DamageFeedback;
	EventData.OptionalObject = MutableDamageInfo;
	EventData.Instigator = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent* AttackerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		DamageInfo->GetSourceActor());
	if (AttackerASC)
		AttackerASC->HandleGameplayEvent(Tag_Combat_Event_AbilityTrigger_DamageFeedback, &EventData);
}

void UCombatAbility_ReceiveHit::HandleOutofHealth(AActor* DamageInstigator, AActor* DamageCauser,
                                                  const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
                                                  float OldValue, float NewValue)
{
	bCharacterDead = true;
}


void UCombatAbility_ReceiveHit::HandleReceiveDamage_Implementation(const FGameplayEventData& Payload)
{
	const UCancerDamageType* DamageInfo = Cast<UCancerDamageType>(Payload.OptionalObject.Get());
	if (!DamageInfo)
	{
		ensureMsgf(DamageInfo, TEXT("没有传递有效的DamageType"));
		K2_EndAbility();
	}
	const AActor* Attacker = DamageInfo->GetSourceActor();
	AActor* Victim = GetOwningActorFromActorInfo();
	auto HitType = DamageInfo->GetHitType();
	auto HitInfo = DamageInfo->GetHitInfo();
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();


	ASC->HandleGameplayEvent(Tag_Combat_Event_AbilityTrigger_Stagger, &Payload);

	if (HitInfo.TriggerTargetAbilityTag.IsValid())
	{
		FGameplayEventData EventData = Payload;
		EventData.EventTag = HitInfo.TriggerTargetAbilityTag;
		EventData.EventMagnitude = HitInfo.TriggerTargetAbilityDuration;
		ASC->HandleGameplayEvent(HitInfo.TriggerTargetAbilityTag, &EventData);
	}

	const FCancerComboInfo& ComboInfo = DamageInfo->DamageParameter.ComboInfo;

	if (auto AbilitySystem = GetWorld()->GetSubsystem<UCancerGlobalAbilitySystem>())
	{
		AbilitySystem->K2_CombatSlomo(const_cast<AActor*>(Attacker), Victim, ComboInfo);
	}
	float HitSoulValue = 0;

	//气力伤害免疫
	if (!HasMatchTag(Tag_Combat_State_InvulnerableSoul))
	{
		HitSoulValue = HitInfo.SoulValue;
	}
	float HitDamageValue = HitInfo.Damage;

	//攻击者恢复气力参数
	auto AttackerASC = UCancerAbilityFunctionLibrary::GetCancerAbilitySystemComponent(const_cast<AActor*>(Attacker));
	auto SoulData = AttackerASC->GetSoulData();
	FVector Soul;
	if (SoulData.ReplyValueMaps.Contains(HitType))
	{
		Soul = *SoulData.ReplyValueMaps.Find(HitType);
	}
	else
	{
		Soul = FVector(0.2, 0.2, 10);
	}

	//虚弱状态伤害增幅
	if (HasMatchTag(Tag_Combat_State_Weak))
	{
		HitDamageValue *= SoulData.DamageCoefficient;
	}

	//施加伤害
	UCancerAbilityFunctionLibrary::K2_ApplyDamage_SetByCaller(
		Victim, DamageInfo, Tag_Combat_Event_AbilityTrigger_ReceiveHit, HitDamageValue, HitSoulValue,
		Soul);

#pragma region 执行伤害GC
	FGameplayTag GCTag = HitInfo.DamageGCTag;
	FGameplayCueParameters Parameters;
	Parameters.NormalizedMagnitude = Payload.EventMagnitude;
	Parameters.Location = HitInfo.HitResult.ImpactPoint;
	Parameters.Normal = HitInfo.HitResult.ImpactNormal;
	Parameters.Instigator = const_cast<AActor*>(Attacker);
	Parameters.EffectCauser = Victim;
	Parameters.SourceObject = DamageInfo;
	UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(Victim, GCTag, Parameters);
#pragma endregion

	SendDamageFeedback(DamageInfo, {});

	K2_EndAbility();
}

void UCombatAbility_ReceiveHit::HandleReceiveBlock_Implementation(const FGameplayEventData& Payload)
{
	UCancerStaticsSubsystem::StaticBlock(GetAvatarActorFromActorInfo());

	if (UCancerHeroComponent* HeroComponent = GetHeroComponentFromActorInfo())
	{
		if (HeroComponent->IncrementBlock())
		{
			HandleReceivePerfectBlock(Payload);
			return;
		}
	}

	const UCancerDamageType* DamageInfo = Cast<UCancerDamageType>(Payload.OptionalObject.Get());
	if (!DamageInfo)
	{
		ensureMsgf(DamageInfo, TEXT("没有传递有效的DamageType"));
		K2_EndAbility();
	}

	const AActor* Attacker = DamageInfo->GetSourceActor();
	AActor* Victim = GetOwningActorFromActorInfo();
	auto HitType = DamageInfo->GetHitType();
	auto HitInfo = DamageInfo->GetHitInfo();

	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();

	//触发格挡
	ASC->HandleGameplayEvent(Tag_Combat_Event_AbilityTrigger_Block, &Payload);

	//攻击者气力恢复参数
	auto AttackerASC = UCancerAbilityFunctionLibrary::GetCancerAbilitySystemComponent(const_cast<AActor*>(Attacker));
	auto AttackerSoulData = AttackerASC->GetSoulData();
	FVector Soul;
	if (AttackerSoulData.ReplyValueMaps.Contains(HitType))
	{
		Soul = *AttackerSoulData.ReplyValueMaps.Find(HitType);
	}
	else
	{
		Soul = FVector(0.2, 0.2, 10);
	}

	//受击者格挡免疫气力参数
	auto VictimSoulData = AttackerASC->GetSoulData();
	// 免疫后气力伤害 = 格挡免疫系数 * 气力伤害系数  
	float FinishSoul = VictimSoulData.SoulBonusPct * HitInfo.SoulValue;
	UCancerAbilityFunctionLibrary::K2_ApplyDamage_SetByCaller(
		Victim, DamageInfo, Tag_Combat_Event_AbilityTrigger_ReceiveHit, 0, FinishSoul,
		Soul);

#pragma region 执行格挡GC
	FCancerHitEffectInfo HitEffectInfo = DamageInfo->GetHitEffectInfo();
	FGameplayTag GCTag = HitEffectInfo.GCBlockType;
	FGameplayCueParameters Parameters;
	Parameters.NormalizedMagnitude = Payload.EventMagnitude;
	Parameters.Location = HitInfo.HitResult.ImpactPoint;
	Parameters.Normal = HitInfo.HitResult.ImpactNormal;
	Parameters.Instigator = const_cast<AActor*>(Attacker);
	Parameters.EffectCauser = Victim;
	Parameters.SourceObject = DamageInfo;
	UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(Victim, GCTag, Parameters);
#pragma endregion


	SendDamageFeedback(DamageInfo, FGameplayTagContainer(Tag_Combat_DamageFeedBack_Block));

	K2_EndAbility();
}

void UCombatAbility_ReceiveHit::HandleReceivePerfectBlock_Implementation(const FGameplayEventData& Payload)
{
	const UCancerDamageType* DamageInfo = Cast<UCancerDamageType>(Payload.OptionalObject.Get());
	if (!DamageInfo)
	{
		ensureMsgf(DamageInfo, TEXT("没有传递有效的DamageType"));
		K2_EndAbility();
	}

	UCancerStaticsSubsystem::StaticPerfectBlock(GetAvatarActorFromActorInfo());

	//触发受击方的弹反技能
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	ASC->HandleGameplayEvent(Tag_Combat_Event_AbilityTrigger_PerfectBlock, &Payload);

#pragma region 执行弹反GC
	const AActor* Attacker = DamageInfo->GetSourceActor();
	AActor* Victim = GetOwningActorFromActorInfo();
	FCancerHitInfo HitInfo = DamageInfo->GetHitInfo();
	FCancerHitEffectInfo HitEffectInfo = DamageInfo->GetHitEffectInfo();
	FGameplayTag GCTag = HitEffectInfo.GCPerfectBlockType;
	FGameplayCueParameters Parameters;
	Parameters.NormalizedMagnitude = Payload.EventMagnitude;
	Parameters.Location = HitInfo.HitResult.ImpactPoint;
	Parameters.Normal = HitInfo.HitResult.ImpactNormal;
	Parameters.Instigator = const_cast<AActor*>(Attacker);
	Parameters.EffectCauser = Victim;
	Parameters.SourceObject = DamageInfo;
	UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(Victim, GCTag, Parameters);
#pragma endregion

	auto AttackerASC = UCancerAbilityFunctionLibrary::
		GetCancerAbilitySystemComponent(const_cast<AActor*>(Attacker));
	auto AttackerAttribute = UCancerAbilityFunctionLibrary::GetCancerCombatAttributeComponent(
		const_cast<AActor*>(Attacker));
	if (!AttackerASC || !AttackerAttribute)
	{
		K2_EndAbility();
		return;
	}

	//反击气力系数
	auto SoulData = ASC->GetSoulData();
	FVector Soul;
	if (SoulData.ReplyValueMaps.Contains(ECancerHitType::StrikeBack))
	{
		Soul = *SoulData.ReplyValueMaps.Find(ECancerHitType::StrikeBack);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("默认恢复气力参数（0.2 ，0.2，10）"))
		Soul = FVector(0.2, 0.2, 10);
	}

	//弹法气力伤害系数
	float SoulCoeff = SoulData.PerfectBlockDamageSoul;

	auto Da = const_cast<UCancerDamageType*>(DamageInfo);
	Da->SetSourceActor(GetOwningActorFromActorInfo());

	UCancerAbilityFunctionLibrary::K2_ApplyDamage_SetByCaller(
		const_cast<AActor*>(Attacker), Da, Tag_Combat_Event_AbilityTrigger_ReceiveHit, 0, SoulCoeff,
		Soul);

	Da->SetSourceActor(const_cast<AActor*>(Attacker));
	SendDamageFeedback(DamageInfo, FGameplayTagContainer(Tag_Combat_DamageFeedBack_PerfectBlock));

	K2_EndAbility();
}

void UCombatAbility_ReceiveHit::HandleReceiveInvulnerable_Implementation(const FGameplayEventData& Payload)
{
	const UCancerDamageType* DamageInfo = Cast<UCancerDamageType>(Payload.OptionalObject.Get());
	if (!DamageInfo)
	{
		ensureMsgf(DamageInfo, TEXT("没有传递有效的DamageType"));
		K2_EndAbility();
	}

	//触发完美闪避
	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	ASC->HandleGameplayEvent(Tag_Combat_Event_AbilityTrigger_PerfectDodge, &Payload);

	SendDamageFeedback(DamageInfo, FGameplayTagContainer(Tag_Combat_DamageFeedBack_Invulnerable));

	const AActor* Attacker = DamageInfo->GetSourceActor();
	AActor* Victim = GetOwningActorFromActorInfo();
	FCancerHitInfo HitInfo = DamageInfo->GetHitInfo();
	FCancerHitEffectInfo HitEffectInfo = DamageInfo->GetHitEffectInfo();
	FGameplayTag GCTag = HitEffectInfo.GCInvulnerableType;
	FGameplayCueParameters Parameters;
	Parameters.NormalizedMagnitude = Payload.EventMagnitude;
	Parameters.Location = HitInfo.HitResult.ImpactPoint;
	Parameters.Normal = HitInfo.HitResult.ImpactNormal;
	Parameters.Instigator = const_cast<AActor*>(Attacker);
	Parameters.EffectCauser = Victim;
	Parameters.SourceObject = DamageInfo;
	UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(Victim, GCTag, Parameters);
	K2_EndAbility();
}
