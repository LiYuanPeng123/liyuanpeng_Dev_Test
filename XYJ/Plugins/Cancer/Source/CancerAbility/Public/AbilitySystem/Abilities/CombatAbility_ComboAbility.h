#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerCombatAnimationAbility.h"
#include "AbilitySystem/Cost/CancerAbilityCost_GameplayEffect.h"
#include "AbilitySystem/Interfaces/CombatAttackAbilityInterface.h"
#include "Data/CancerCombatData.h"
#include "CombatAbility_ComboAbility.generated.h"


class ULinkComboProvider;
class UInputAction;
class UAnimMontage;


USTRUCT(BlueprintType, Blueprintable)
struct FCombatInput
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="输入对应事件标签")
	FGameplayTag CombatInputTag = FGameplayTag();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="输入按键")
	UInputAction* InputAction = nullptr;
};

// 攻击阶段（用于区分普通连击、前摇和蓄力释放）
UENUM(BlueprintType, Blueprintable)
enum class EComboAttackPhase : uint8
{
	None,
	NormalCombo,
	ChargePre,
	ChargedAttack
};


/**
 *  近战连击能力
 */
UCLASS(BlueprintType, Blueprintable)
class CANCERABILITY_API UCombatAbility_ComboAbility : public UCancerGameplayAbility
{
	GENERATED_BODY()

public:
	UCombatAbility_ComboAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;


	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload) override;


	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

#pragma region 蒙太奇的回调事件
	UFUNCTION(BlueprintNativeEvent)
	void OnAnimationStarted();
	UFUNCTION(BlueprintNativeEvent)
	void OnAnimationFinished();
	UFUNCTION(BlueprintNativeEvent)
	void OnAnimationCancelled();
	UFUNCTION(BlueprintNativeEvent)
	void OnAnimationReady();
#pragma endregion

#pragma  region 伤害检测

	virtual void HandleMeleeScanTargetsReceived_Implementation(const UCancerMeleeScan* MeleeScan,
	                                                           const FGameplayAbilityTargetDataHandle& Data) override;
	UPROPERTY()
	UAbilityTask* AnimationTask;
#pragma endregion

#pragma region 连招派生
	// 派生窗口数据
	UPROPERTY()
	ULinkComboProvider* CacheLinkComboProvider;

	// 派生窗口期间是否命中（用于 bISHit 条件判定）
	UPROPERTY()
	bool bHitDuringLinkWindow = false;
#pragma endregion

	void SetComboTag(const FGameplayTag& InComboTag);

protected:
	// 当前攻击阶段
	UPROPERTY(BlueprintReadOnly)
	EComboAttackPhase CurrentAttackPhase = EComboAttackPhase::None;

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetComboEventFromInputAction(const UInputAction* InputAction) const;

	UPROPERTY(BlueprintReadOnly, Transient)
	float ChargeSharedDuration;
	UPROPERTY(BlueprintReadOnly, Transient)
	float ChargeSuccessThreshold;
	UPROPERTY(BlueprintReadOnly, Transient)
	float ChargeMaxDuration;

	UPROPERTY(Transient)
	float HeldDuration;

#pragma region 输入与延时任务
	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitInputRelease> WaitReleaseTask;
	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitDelay> SharedDurationDelayTask;
	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitDelay> MaxChargeDelayTask;
	UPROPERTY()
	TObjectPtr<class UAbilityTask_ChargedAttackCost> ChargeCostTask;

	UFUNCTION()
	void OnSharedDurationElapsed();

	// 输入松手回调：在达到阈值时切入蓄力释放，否则普通连段
	UFUNCTION()
	void HandleInputReleased(float TimeHeld);
	// 播放前摇（蓄力开始），若不支持蓄力则直接转普通连段
	void StartChargePreAnimation();
	// 从前摇过渡到普通连段（播放当前步的普通攻击）
	void TransitionToComboFromPre();
	UFUNCTION()
	// 触发蓄力释放（达到阈值或最大蓄力时间到）
	void TransitionToChargedAttack();
	UFUNCTION()
	void OnChargeCostFailed();

	virtual void AbilityInputReleased_Implementation() override;
#pragma endregion

#pragma region 动画兜底结束
	// 当最终攻击动画完成事件未触发时，使用超时兜底结束能力
	// 动画兜底定时器任务（防止未触发完成事件时能力卡死）
	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitDelay> AnimationTimeoutTask;
	void StartAnimationTimeout(const UAnimMontage* Montage, float PlayRate = 1.0f);
	UFUNCTION()
	void OnAnimationTimeout();
#pragma endregion

	// 获取当前连击数据
	FWeaponData GetCurrentWeaponComboStep();

	//获取派生招式
	FWeaponData GetLinkComboWeaponStep(const FGameplayTag& InLinkComboTag) const;


	// 开始当前段攻击
	void TryBeginAttackForCurrentStep(bool bResetTasks);

	// 按武器步骤配置蓄力参数与任务
	void SetupChargeTasksFromStep(const FWeaponData Step, bool bResetTasks = true);

protected:
	// 动画任务绑定
	void BindDelegatesToAnimationTask(UAbilityTask* Task);
	UAbilityTask* PlayMontageTask(const FName& InstanceName, UAnimMontage* Montage, float Rate = 1.0f,
	                              bool bStopWhenAbilityEnds = true, float RMScale = 1.0f);
	void StartDelayTask(TObjectPtr<class UAbilityTask_WaitDelay>& OutTask, float Duration);

protected:
	
	double PressStartWorldTime = 0.0;

	bool bInputReleased = false;

	bool bPerformedFinalAttack = false;

	UPROPERTY(BlueprintReadOnly)
	bool bUsingOverrideStep = false;
	UPROPERTY(BlueprintReadOnly)
	FWeaponData OverrideStep;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = Combo, DisplayName="蓄力消耗")
	TArray<TObjectPtr<UCancerAbilityCost>> ChargedAttackCosts;
	UPROPERTY(EditDefaultsOnly, Category = Combo, DisplayName="启用蓄力检测",meta=(InlineEditConditionToggle))
	bool CustomChargedAttackCheck = false;
	UPROPERTY(EditAnywhere, Category = Combo, DisplayName = "蓄力检测",
		meta=(EditCondition = "CustomChargedAttackCheck", EditConditionHides))
	TArray<FCancerCostCheck> CustomChargedAttackChecks;

	UFUNCTION()
	bool ChargedAttackCheck();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Combo)
	TArray<FCombatInput> CombatInputs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combo,
		meta=(RowType="/Script/CancerAbility.CancerComboData",
			RequiredAssetDataTags="RowStructure=/Script/CancerAbility.CancerComboData"))
	FDataTableRowHandle CombatInputsHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combo)
	FGameplayTag ComboTag;

	FCancerComboWXData* ComboDta;
};
