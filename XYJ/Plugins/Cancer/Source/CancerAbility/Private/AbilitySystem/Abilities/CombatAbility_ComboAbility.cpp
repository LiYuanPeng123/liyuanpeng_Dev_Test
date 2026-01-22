#include "AbilitySystem/Abilities/CombatAbility_ComboAbility.h"
#include "AbilitySystemComponent.h"
#include "InputAction.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/Tasks/AbilityTask_PlayCombatAnimation.h"
#include "Components/CancerHeroComponent.h"
#include "Types/FCancerCombatAttackSupport.h"
#include "GameFramework/Controller.h"
#include "CancerAbilitySystemComponent.h"
#include "AbilitySystem/Tasks/AbilityTask_ChargedAttackCost.h"
#include "Engine/Engine.h"
#include "GameFramework/CancerCombo.h"
#include "Providers/LinkCombo/LinkComboProvider.h"

DEFINE_LOG_CATEGORY_STATIC(LogCombatAbility_MeleeCombo, Log, All);

UE_DISABLE_OPTIMIZATION

UCombatAbility_ComboAbility::UCombatAbility_ComboAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AnimationTask = nullptr;
	CurrentAttackPhase = EComboAttackPhase::None;
	CacheLinkComboProvider = nullptr;
}


void UCombatAbility_ComboAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (TriggerEventData)
	{
		const UCancerCombo* Combo = Cast<UCancerCombo>(TriggerEventData->OptionalObject);
		SetComboTag(Combo->ComboTag);
	}
	if (auto HeroComponent = UCancerHeroComponent::FindHeroComponent(GetCurrentActorInfo()->AvatarActor.Get()))
	{
		//重置连招
		HeroComponent->ResetCombo();
	}
	bPerformedFinalAttack = false;
	TryBeginAttackForCurrentStep(true);
}


void UCombatAbility_ComboAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	FinishLatentTask(WaitReleaseTask);
	FinishLatentTask(SharedDurationDelayTask);
	FinishLatentTask(MaxChargeDelayTask);
	FinishLatentTask(ChargeCostTask);
	FinishLatentTask(AnimationTimeoutTask);
	if (auto ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(Tag_Combat_State_CombatWindow);
		ASC->RemoveLooseGameplayTag(Tag_Combat_State_ComboLinkWindow);
	}
	if (auto HeroComponent = UCancerHeroComponent::FindHeroComponent(GetCurrentActorInfo()->AvatarActor.Get()))
	{
		//重置连招
		HeroComponent->ResetCombo();
	}
	CurrentAttackPhase = EComboAttackPhase::None;
}


void UCombatAbility_ComboAbility::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);
	//连击窗口
	if (Payload.EventTag == Tag_Combat_Event_CombatWindow_Start)
	{
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(Tag_Combat_State_CombatWindow);
	}
	else if (Payload.EventTag == Tag_Combat_Event_CombatWindow_Stop)
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(Tag_Combat_State_CombatWindow);
	}
	//派生窗口
	if (Payload.EventTag == Tag_Combat_Event_ComboLinkWindow_Start)
	{
		if (const ULinkComboProvider* ProviderConst = Cast<ULinkComboProvider>(Payload.OptionalObject))
		{
			CacheLinkComboProvider = const_cast<ULinkComboProvider*>(ProviderConst);
		}
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(Tag_Combat_State_ComboLinkWindow);
		//重置命中标记
		bHitDuringLinkWindow = false;
	}
	else if (Payload.EventTag == Tag_Combat_Event_ComboLinkWindow_Stop)
	{
		CacheLinkComboProvider = nullptr;
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(Tag_Combat_State_ComboLinkWindow);
		// 清理命中标记
		bHitDuringLinkWindow = false;
	}
	//按键触发
	else if (Payload.EventTag == Tag_Combat_Event_Input_Start)
	{
		const UInputAction* InputAction = Cast<UInputAction>(Payload.OptionalObject);
		auto EventTag = GetComboEventFromInputAction(InputAction);

		//连招派生
		if (IsValid(CacheLinkComboProvider) && CacheLinkComboProvider->InputAction == InputAction)
		{
			FGameplayTag MontageTag = CacheLinkComboProvider->LinkComboTag;
			const bool bRequireHit = CacheLinkComboProvider->bISHit;
			/*if (EventTag == Tag_Combat_Event_Attack_Light_R)
			{*/
			if (HasMatchTag(Tag_Combat_State_ComboLinkWindow))
			{
				// 若需要命中，则必须在派生窗口期间发生命中
				if (!bRequireHit || bHitDuringLinkWindow)
				{
					// 切换到派生步骤
					OverrideStep = GetLinkComboWeaponStep(MontageTag);
					if (OverrideStep.MontageTag.IsValid())
					{
						bPerformedFinalAttack = false;
						bUsingOverrideStep = true;
						TryBeginAttackForCurrentStep(true);
						UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("派生连招触发：%s (需要命中=%s, 已命中=%s)"),
						       *MontageTag.ToString(), bRequireHit ? TEXT("是") : TEXT("否"),
						       bHitDuringLinkWindow ? TEXT("是") : TEXT("否"));
					}
					else
					{
						UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("未找到派生连招步骤：%s"), *MontageTag.ToString());
					}
				}
				else
				{
					UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("派生连招未触发：需要命中但派生窗口内未命中"));
				}
				/*}*/
			}
			return;
		}
		//连击
		if (EventTag == Tag_Combat_Event_Attack_Light_L)
		{
			if (auto HeroComponent = UCancerHeroComponent::FindHeroComponent(
				GetCurrentActorInfo()->AvatarActor.Get()))
			{
				if (HasMatchTag(Tag_Combat_State_CombatWindow))
				{
					bPerformedFinalAttack = false;
					TryBeginAttackForCurrentStep(/*bResetTasks*/true);
					UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("%s-->衔接下一段"), *EventTag.ToString());
				}
			}
		}
	}
}

void UCombatAbility_ComboAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (CombatInputsHandle.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("UCombatAbility_ComboAbility::OnGiveAbility --技能初始化数据失败"))
	}
	ComboDta = CombatInputsHandle.GetRow<FCancerComboWXData>("ComboAbility");
}


#pragma region 蒙太奇回调
void UCombatAbility_ComboAbility::OnAnimationStarted_Implementation()
{
	// 根据当前阶段处理连击计数
	if (auto HeroCombatComponent = GetHeroComponentFromActorInfo())
	{
		switch (CurrentAttackPhase)
		{
		case EComboAttackPhase::NormalCombo:
			HeroCombatComponent->IncrementCombo();
			break;
		case EComboAttackPhase::ChargePre:
			HeldDuration = 0;
		case EComboAttackPhase::ChargedAttack:
		default:
			break;
		}
	}
}

void UCombatAbility_ComboAbility::OnAnimationReady_Implementation()
{
}

void UCombatAbility_ComboAbility::OnAnimationFinished_Implementation()
{
	if (bPerformedFinalAttack)
	{
		UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("连招动画完成，结束能力"));
		K2_EndAbility();
	}
}

// 动画取消/打断：立即结束能力
void UCombatAbility_ComboAbility::OnAnimationCancelled_Implementation()
{
	UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("连招动画被打断，结束能力"));
	K2_EndAbility();
}

#pragma endregion
#pragma region 伤害命中
void UCombatAbility_ComboAbility::HandleMeleeScanTargetsReceived_Implementation(const UCancerMeleeScan* MeleeScan,
	const FGameplayAbilityTargetDataHandle&
	Data)
{
	//传递蓄力时长 HeldDuration 
	if (IsValid(MeleeScan) && MeleeScan->DamageType)
	{
		if (CurrentAttackPhase == EComboAttackPhase::ChargedAttack)
		{
			const float Damage = MeleeScan->DamageType->DamageParameter.HitInfo.Damage;
			const auto& CurveFloat = MeleeScan->DamageType->GetHitInfo().ChargeupDamage;
			float DamageBuffer = 1;
			if (const FRichCurve* RichCurve = CurveFloat.GetRichCurveConst())
			{
				DamageBuffer = RichCurve->Eval(HeldDuration);
			}
			const float FinalDamage = Damage * DamageBuffer + Damage;
			MeleeScan->DamageType->DamageParameter.HitInfo.Damage = FinalDamage;
			UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("蓄力命中[时长：%f][伤害增幅:%f] 最终伤害[%f]"), HeldDuration,
			       DamageBuffer, FinalDamage);
		}
	}

	//TODO::
	//伤害计算前覆盖
	Super::HandleMeleeScanTargetsReceived_Implementation(MeleeScan, Data);


	// 在派生窗口期间发生命中
	if (HasMatchTag(Tag_Combat_State_ComboLinkWindow))
	{
		bHitDuringLinkWindow = true;
	}
}

#pragma endregion
#pragma region 辅助函数

void UCombatAbility_ComboAbility::SetComboTag(const FGameplayTag& InComboTag)
{
	ComboTag = InComboTag;
}

FGameplayTag UCombatAbility_ComboAbility::GetComboEventFromInputAction(const UInputAction* InputAction) const
{
	for (auto InputCombat : CombatInputs)
	{
		if (InputCombat.InputAction == InputAction)
		{
			return InputCombat.CombatInputTag;
		}
	}
	return FGameplayTag();
}


// 统一绑定动画任务委托
void UCombatAbility_ComboAbility::BindDelegatesToAnimationTask(UAbilityTask* Task)
{
	if (!IsValid(Task) || !Task->Implements<UCancerCombatAnimationTaskInterface>())
	{
		return;
	}

	FCombatAnimationEventDelegate ReadyDelegate;
	ReadyDelegate.BindDynamic(this, &ThisClass::OnAnimationReady);
	FCombatAnimationEventDelegate StartedDelegate;
	StartedDelegate.BindDynamic(this, &ThisClass::OnAnimationStarted);
	FCombatAnimationEventDelegate CompletedDelegate;
	CompletedDelegate.BindDynamic(this, &ThisClass::OnAnimationFinished);
	FCombatAnimationEventDelegate CancelledDelegate;
	CancelledDelegate.BindDynamic(this, &ThisClass::OnAnimationCancelled);
	FCombatAnimationEventDelegate InterruptedDelegate;
	InterruptedDelegate.BindDynamic(this, &ThisClass::OnAnimationCancelled);

	ICancerCombatAnimationTaskInterface::Execute_BindToAnimationReady(Task, ReadyDelegate);
	ICancerCombatAnimationTaskInterface::Execute_BindToAnimationStarted(Task, StartedDelegate);
	ICancerCombatAnimationTaskInterface::Execute_BindToAnimationCompleted(Task, CompletedDelegate);
	ICancerCombatAnimationTaskInterface::Execute_BindToAnimationCancelled(Task, CancelledDelegate);
	ICancerCombatAnimationTaskInterface::Execute_BindToAnimationInterrupted(Task, InterruptedDelegate);
}
#pragma endregion

UAbilityTask* UCombatAbility_ComboAbility::PlayMontageTask(const FName& InstanceName, UAnimMontage* Montage, float Rate,
                                                           bool bStopWhenAbilityEnds, float RMScale)
{
	UAbilityTask* LastTask = AnimationTask;
	AnimationTask = UAbilityTask_PlayCombatAnimation::CreatePlayCombatAnimationProxy(
		this, InstanceName, Montage, Rate, NAME_None, bStopWhenAbilityEnds, RMScale);
	if (IsValid(AnimationTask))
	{
		FinishLatentTask(LastTask);
	}
	BindDelegatesToAnimationTask(AnimationTask);
	if (IsValid(AnimationTask))
	{
		AnimationTask->ReadyForActivation();
		StartAnimationTimeout(Montage, Rate);
		bUsingOverrideStep = false;
	}
	return AnimationTask;
}

void UCombatAbility_ComboAbility::StartDelayTask(TObjectPtr<UAbilityTask_WaitDelay>& OutTask, float Duration)
{
	OutTask = UAbilityTask_WaitDelay::WaitDelay(this, Duration);
	if (IsValid(OutTask))
	{
		OutTask->ReadyForActivation();
	}
}

bool UCombatAbility_ComboAbility::ChargedAttackCheck()
{
	auto CheckBaseValue = [](ECancerCostCheckType CheckType, float
	                         Value, float TargetValue) -> bool
	{
		switch (CheckType)
		{
		case ECancerCostCheckType::Equal:
			return Value == TargetValue;
		case ECancerCostCheckType::Greater:
			return Value > TargetValue;
		case ECancerCostCheckType::GreaterOrEqual:
			return Value >= TargetValue;
		case ECancerCostCheckType::Less:
			return Value < TargetValue;
		case ECancerCostCheckType::LessOrEqual:
			return Value <= TargetValue;
		case ECancerCostCheckType::NotEqual:
			return Value != TargetValue;
		default: return true;
		}
	};

	auto ASC = GetCancerAbilitySystemComponentFromActorInfo();
	//执行自定义的检测
	for (auto CostCheck : CustomChargedAttackChecks)
	{
		if (CostCheck.MustTarget)
		{
			const float Value = ASC->GetNumericAttribute(CostCheck.CheckAttribute);
			const float TargetValue = ASC->GetNumericAttribute(CostCheck.CheckAttributeBase);
			float NewValue = Value / TargetValue;
			if (CheckBaseValue(CostCheck.CostCheckType, NewValue, CostCheck.PercentValue))
			{
				continue;
			}
			else
			{
				return false;
			}
		}
		else
		{
			const float Value = ASC->GetNumericAttribute(CostCheck.CheckAttribute);
			if (CheckBaseValue(CostCheck.CostCheckType, Value, CostCheck.Value))
			{
				continue;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}


void UCombatAbility_ComboAbility::HandleInputReleased(float TimeHeld)
{
}


// 播放前摇（蓄力准备），若不支持蓄力直接转普通段
void UCombatAbility_ComboAbility::StartChargePreAnimation()
{
	UAnimMontage* PreMontage = nullptr;
	const FWeaponData Step = GetCurrentWeaponComboStep();
	bool bStrength = false;

	if (Step.bIsStrength)
	{
		if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponentFromActorInfo())
		{
			if (Step.PreMontageTag.IsValid())
			{
				PreMontage = ASC->GetAbilityMontage(Step.PreMontageTag);
				check(PreMontage);
				if (PreMontage)
				{
					bStrength = true;
				}
			}
		}
	}

	//不支持蓄力
	if (!bStrength)
	{
		TransitionToComboFromPre();
		return;
	}

	// 设置阶段为前摇
	CurrentAttackPhase = EComboAttackPhase::ChargePre;

	static const FName TaskInstanceName = TEXT("PlayChargePre");
	PlayMontageTask(TaskInstanceName, PreMontage, 1.0f, true, 1.0f);

}

// 从前摇过渡到普通连段：播放当前步普通攻击并启动兜底定时器
void UCombatAbility_ComboAbility::TransitionToComboFromPre()
{
	if (bPerformedFinalAttack)
	{
		return;
	}
	bPerformedFinalAttack = true;
	CurrentAttackPhase = EComboAttackPhase::NormalCombo; // 设置阶段为普通连击
	UAnimMontage* MontageToPlay = nullptr;
	const FWeaponData Step = GetCurrentWeaponComboStep();

	if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponentFromActorInfo())
	{
		if (Step.MontageTag.IsValid())
		{
			MontageToPlay = ASC->GetAbilityMontage(Step.MontageTag);
		}
	}

	// 没有动画，直接结束能力
	if (!IsValid(MontageToPlay))
	{
		UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("普通连击无有效动画,结束能力"));
		K2_EndAbility();
		return;
	}

	static const FName TaskInstanceName = TEXT("NormalAttack");
	if (!IsValid(PlayMontageTask(TaskInstanceName, MontageToPlay, 1.0f, true, 1.0f)))
	{
		UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("创建普通连击动画任务失败,结束能力"));
		K2_EndAbility();
		return;
	}

	//结束蓄力消耗
	FinishLatentTask(ChargeCostTask);
}


// 触发蓄力释放：播放释放动画并启动兜底定时器
void UCombatAbility_ComboAbility::TransitionToChargedAttack()
{
	if (bPerformedFinalAttack)
	{
		return;
	}

	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	HeldDuration = Now - PressStartWorldTime;
	// 仅当满足成功阈值时才触发蓄力攻击（达到最大蓄力时间的 OnFinish 也会调用本方法）
	if (HeldDuration < static_cast<double>(ChargeSuccessThreshold))
	{
		// 若未达到成功阈值（例如成功阈值配置大于最大蓄力时间），
		// 则退化到普通连击，避免能力停留在前摇阶段无法结束。
		UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("蓄力不足（%.3f < 阈值 %.3f），退化到普通连击"),
		       HeldDuration, static_cast<double>(ChargeSuccessThreshold));
		FinishLatentTask(MaxChargeDelayTask);
		FinishLatentTask(SharedDurationDelayTask);
		TransitionToComboFromPre();
		return;
	}

	// 优先使用武器数据的释放标签
	UAnimMontage* ChargedMontage = nullptr;
	// 使用当前 ComboIndex 指向的段（在衔接时已指向下一段）
	const FWeaponData Step = GetCurrentWeaponComboStep();

	if (Step.bIsStrength)
	{
		if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponentFromActorInfo())
		{
			if (Step.PostMontageTag.IsValid())
			{
				ChargedMontage = ASC->GetAbilityMontage(Step.PostMontageTag);
			}
		}
	}
	if (!IsValid(ChargedMontage))
	{
		// 未配置蓄力攻击动画则退化为普通连击
		TransitionToComboFromPre();
		return;
	}
	bPerformedFinalAttack = true;
	// 设置阶段为蓄力释放
	CurrentAttackPhase = EComboAttackPhase::ChargedAttack;
	FinishLatentTask(SharedDurationDelayTask);
	FinishLatentTask(MaxChargeDelayTask);

	static const FName TaskInstanceName = TEXT("PlayChargedAttack");
	PlayMontageTask(TaskInstanceName, ChargedMontage, 1.0f, true, 1.0f);
	
}

void UCombatAbility_ComboAbility::AbilityInputReleased_Implementation()
{
	Super::AbilityInputReleased_Implementation();
	bInputReleased = true;
	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	const double InputHeldDuration = Now - PressStartWorldTime;

	UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("-》》按键松开"));
	if (InputHeldDuration >= static_cast<double>(ChargeSuccessThreshold))
	{
		TransitionToChargedAttack();
		return;
	}
	if (InputHeldDuration >= static_cast<double>(ChargeSharedDuration))
	{
		FinishLatentTask(SharedDurationDelayTask);
		TransitionToComboFromPre();
		return;
	}
}

FWeaponData UCombatAbility_ComboAbility::GetCurrentWeaponComboStep()
{
	if (bUsingOverrideStep && OverrideStep.MontageTag.IsValid())
	{
		return OverrideStep;
	}
	UCancerHeroComponent* HeroComp = GetHeroComponentFromActorInfo();
	const int32 ComboIndex = HeroComp ? HeroComp->ComboIndex : 0;


	check(ComboDta);

	if (!ComboDta)
		return FWeaponData();
	FWeaponWXData* WXData = ComboDta->WeaponInfo.Find(ComboTag);
	if (!WXData)
		return FWeaponData();

	if (HasMatchTag(Tag_Combat_State_PerfectBlockLinkWindow))
	{
		//弹反派生技能
		return WXData->PerfectBlockLinkCombo;
	}

	if (HasMatchTag(Tag_Combat_State_DodgeLinkWindow))
	{
		//闪避派生技能
		return WXData->DodgeLinkCombo;
	}


	TArray<FWeaponData> WeaponDatas = WXData->WeaponDatas;
	if (!WeaponDatas.IsValidIndex(ComboIndex))
	{
		UE_LOG(LogCombatAbility_MeleeCombo, Error, TEXT("无效的index 重置连招为0 %d[]"), ComboIndex);
		HeroComp->ResetCombo();
		if (WeaponDatas.IsValidIndex(0))
			return WeaponDatas[0];
	}
	return WeaponDatas[ComboIndex];
}


FWeaponData UCombatAbility_ComboAbility::GetLinkComboWeaponStep(const FGameplayTag& InLinkComboTag) const
{
	if (!ComboDta) return FWeaponData();
	FWeaponWXData* WXData = ComboDta->WeaponInfo.Find(ComboTag);
	if (!WXData)
		return FWeaponData();
	for (auto WeaponDta : WXData->WeaponLinkDatas)
	{
		if (WeaponDta.MontageTag == InLinkComboTag)
		{
			return WeaponDta;
		}
	}
	return FWeaponData();
}

// 根据武器步骤配置蓄力相关参数与任务，并重置旧任务
void UCombatAbility_ComboAbility::SetupChargeTasksFromStep(const FWeaponData Step, bool bResetTasks)
{
	// 从武器步骤配置蓄力参数
	ChargeSharedDuration = Step.StrengthTime;
	ChargeSuccessThreshold = Step.DetermineDuration;
	ChargeMaxDuration = Step.MaximumChargingTime;

	// 清理旧任务，避免重复绑定
	if (bResetTasks)
	{
		FinishLatentTask(WaitReleaseTask);
		FinishLatentTask(SharedDurationDelayTask);
		FinishLatentTask(MaxChargeDelayTask);
	}

	// 记录时间与状态
	PressStartWorldTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	bInputReleased = false;

	// 前摇共用时长
	StartDelayTask(SharedDurationDelayTask, ChargeSharedDuration);
	if (IsValid(SharedDurationDelayTask))
	{
		SharedDurationDelayTask->OnFinish.AddDynamic(this, &ThisClass::OnSharedDurationElapsed);
	}

	// 最大蓄力时长
	StartDelayTask(MaxChargeDelayTask, ChargeMaxDuration);
	if (IsValid(MaxChargeDelayTask))
	{
		MaxChargeDelayTask->OnFinish.AddDynamic(this, &ThisClass::TransitionToChargedAttack);
	}
}

void UCombatAbility_ComboAbility::OnSharedDurationElapsed()
{
	if (bPerformedFinalAttack)
	{
		return;
	}
	if (bInputReleased)
	{
		UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("前摇共用时长结束且已松手 -> 切到普通连段"));
		TransitionToComboFromPre();
	}
}

void UCombatAbility_ComboAbility::TryBeginAttackForCurrentStep(bool bResetTasks)
{
	const FWeaponData Step = GetCurrentWeaponComboStep();
	UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponentFromActorInfo();
	if (Step.bIsStrength)
	{
		if (CustomChargedAttackCheck)
		{
			if (!ChargedAttackCheck())
			{
				UE_LOG(LogTemp, Warning, TEXT("TryBeginAttackForCurrentStep --蓄力检查 -- 失败"));
				K2_EndAbility();
				return;
			}
		}
		//蓄力消耗
		ChargeCostTask = UAbilityTask_ChargedAttackCost::ChargedAttackCost(this, ChargedAttackCosts);
		if (IsValid(ChargeCostTask))
		{
			ChargeCostTask->OnCostFailed.AddDynamic(this, &ThisClass::OnChargeCostFailed);
			ChargeCostTask->ReadyForActivation();
		}
		// 配置蓄力任务，并播放前摇
		SetupChargeTasksFromStep(Step, bResetTasks);
		StartChargePreAnimation();
	}
	else
	{
		TransitionToComboFromPre();
	}
}

UE_ENABLE_OPTIMIZATION
 
void UCombatAbility_ComboAbility::OnChargeCostFailed()
{
	if (bPerformedFinalAttack)
	{
		return;
	}
	TransitionToComboFromPre();
}

// --- 兜底结束实现 ---
void UCombatAbility_ComboAbility::StartAnimationTimeout(const UAnimMontage* Montage, float PlayRate)
{
	FinishLatentTask(AnimationTimeoutTask);
	if (!IsValid(Montage) || PlayRate <= 0.f)
	{
		return;
	}
	const float Duration = Montage->GetPlayLength() / PlayRate;
	float ActorDilation = 1.0f;
	if (const AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		ActorDilation = FMath::Clamp(Avatar->CustomTimeDilation, 0.01f, 10.0f);
	}
	const float Timeout = FMath::Max((Duration / ActorDilation) + 0.1f, 0.01f);
	AnimationTimeoutTask = UAbilityTask_WaitDelay::WaitDelay(this, Timeout);
	if (IsValid(AnimationTimeoutTask))
	{
		AnimationTimeoutTask->OnFinish.AddDynamic(this, &ThisClass::OnAnimationTimeout);
		AnimationTimeoutTask->ReadyForActivation();
	}
}

// 兜底回调：仅在最终攻击阶段结束能力，避免前摇阶段误结束
void UCombatAbility_ComboAbility::OnAnimationTimeout()
{
	// 仅在最终攻击阶段使用兜底结束，避免前摇阶段误结束
	if (bPerformedFinalAttack)
	{
		UE_LOG(LogCombatAbility_MeleeCombo, Warning, TEXT("最终动画超时兜底，结束能力"));
		K2_EndAbility();
	}
}
