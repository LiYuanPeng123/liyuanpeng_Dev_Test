#pragma once

#include "CoreMinimal.h"
#include "CameraAnimationCameraModifier.h"
#include "Abilities/GameplayAbility.h"
#include "CancerCamera/Public/CancerCameraMode.h"
#include "Components/CancerMovementComponent.h"
#include "Interfaces/CombatAttackAbilityInterface.h"
#include "Types/FCombatAbilitySet.h"
#include "UObject/Object.h"
#include "CancerGameplayAbility.generated.h"

class UCancerLockingTargetComponent;
class UAbilityDef;
class UCancerCombatAttributeComponent;
class UCancerStaggerComponent;
class UCancerCosmeticsComponent;
class UCancerMotionWarpingComponent;
class UCancerAbilitySystemComponent;
class UCancerHeroComponent;
class UCancerAbilityCost;
class UAbilityTask_WaitGameplayEvent;


/**
 * ECancerAbilityActivationPolicy
 *
 *	定义了该能力应如何被激活的方式。
 */
UENUM(BlueprintType)
enum class ECancerAbilityActivationPolicy : uint8
{
	None = 0,
	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

/**
 *  技能基类
 */
UCLASS()
class CANCERABILITY_API UCancerGameplayAbility : public UGameplayAbility,
                                                 public ICombatAttackAbilityInterface
{
	GENERATED_BODY()

public:
#pragma  region 辅助函数

	template <class T = AActor>
	T* GetActor() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AActor>::Value,
		              "'T' template parameter to GetActor must be derived from AActor");
		return Cast<T>(GetOwningActorFromActorInfo());
	}

	template <class T = AActor>
	T* GetActorChecked() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AActor>::Value,
		              "'T' template parameter to GetActorChecked must be derived from AActor");
		return CastChecked<T>(GetOwningActorFromActorInfo());
	}

	template <class T = APawn>
	T* GetPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value,
		              "'T' template parameter to GetPawn must be derived from APawn");
		return Cast<T>(GetOwningActorFromActorInfo());
	}

	template <class T = APawn>
	T* GetPawnChecked() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value,
		              "'T' template parameter to GetPawnChecked must be derived from APawn");
		return CastChecked<T>(GetOwningActorFromActorInfo());
	}

	template <class T = APlayerState>
	T* GetPlayerState() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APlayerState>::Value,
		              "'T' template parameter to GetPlayerState must be derived from APlayerState");
		return GetPawnChecked<APawn>()->GetPlayerState<T>();
	}

	template <class T = AController>
	T* GetController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value,
		              "'T' template parameter to GetController must be derived from AController");
		if (auto Pawn = GetPawn<APawn>())
		{
			return Pawn->GetController<T>();
		}
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Cancer Ability",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	class AActor* K2_GetOwnerActor(bool& bIsValid, TSubclassOf<AActor> OutputClass = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Cancer Ability",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	class AController* K2_GetOwnerController(bool& bIsValid, TSubclassOf<AController> OutputClass = nullptr);

	UFUNCTION(BlueprintCallable)
	bool IsPawn() const;
	UFUNCTION(BlueprintCallable)
	bool IsAIControlled() const;

	UFUNCTION(BlueprintCallable, Category = "Cancer |Abilities")
	UCancerHeroComponent* GetHeroComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Cancer |Abilities")
	UAnimInstance* GetAnimInstanceFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Cancer |Abilities")
	UCancerMotionWarpingComponent* GetMotionWarpingComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Cancer |Abilities")
	UCancerCosmeticsComponent* GetCosmeticsComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Cancer |Abilities")
	UCancerMovementComponent* GetCancerMovementFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Cancer |Abilities")
	UCancerLockingTargetComponent* GetLockingTargetComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Cancer|Ability")
	UCancerAbilitySystemComponent* GetCancerAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Cancer|Ability")
	UCancerCombatAttributeComponent* GetCancerCombatAttributeComponentFromActorInfo() const;


#pragma endregion

	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                         const FGameplayAbilityActivationInfo ActivationInfo,
	                         FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	                         const FGameplayEventData* TriggerEventData = nullptr) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags = nullptr,
	                                const FGameplayTagContainer* TargetTags = nullptr,
	                                FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;
	UCancerGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                       OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual void PostInitProperties() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability", DisplayName="技能激活组")
	ECancerAbilityActivationPolicy ActivationPolicy;

	ECancerAbilityActivationPolicy GetActivationPolicy() const
	{
		return ActivationPolicy;
	};
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Combat Ability|Costs", DisplayName="技能启动消耗")
	TArray<TObjectPtr<UCancerAbilityCost>> AdditionalCosts;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Combat Ability|Costs", DisplayName="动作消耗")
	TArray<TObjectPtr<UCancerAbilityCost>> AnimationCosts;

	//提交动作消耗
	bool CommitAnimationCost();
	//执行动作检测
	UFUNCTION(BlueprintNativeEvent)
	bool K2_CanExecuteAnimation();


	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();


	// Sets the ability's camera mode.
	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	void SetCameraMode(TSubclassOf<UCancerCameraMode> CameraMode);

	// Clears the ability's camera mode.  Automatically called if needed when the ability ends.
	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	void ClearCameraMode();


	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	UAbilityTask_WaitGameplayEvent* InitializeEventTask(FGameplayTag Event, bool bOnlyMatchExact = true);

	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	static void FinishLatentTasks(const TArray<UAbilityTask*>& Tasks);

	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	static void FinishLatentTask(UAbilityTask* Task);

	UFUNCTION(BlueprintNativeEvent, Category = "Combat Ability")
	void HandleEventReceived(FGameplayEventData Payload);
	virtual void HandleEventReceived_Implementation(FGameplayEventData Payload);

	UFUNCTION(BlueprintCallable, Category = Combat)
	bool HasMatchTag(FGameplayTag Tag) const;


	virtual bool CanReceivedInput(const FGameplayEventData& Payload) const;


#pragma region 伤害检测

	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	virtual void InitializeMeleeScanTask(const UCancerMeleeScan* MeleeScan) override;
	UFUNCTION(BlueprintCallable, Category = "Combat Ability")
	virtual UAbilityTask_ScanMeleeTarget* GetMeleeScanTask() const override;

	UFUNCTION(BlueprintNativeEvent, Category = "Attack")
	void HandleMeleeScanTargetsReceived(const UCancerMeleeScan* MeleeScan,
	                                    const FGameplayAbilityTargetDataHandle& Data);


	UPROPERTY()
	TObjectPtr<UAbilityTask_ScanMeleeTarget> MeleeScanTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitEventTask;

#pragma endregion

#pragma region 冷却

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual FGameplayTagContainer* GetCooldownTags() const override;

private:
#pragma endregion

#pragma region 输入

public:
	UFUNCTION(BlueprintNativeEvent, Category = Combat)
	void AbilityInputTriggered();

	UFUNCTION(BlueprintNativeEvent, Category = Combat)
	void AbilityInputStarted();

	UFUNCTION(BlueprintNativeEvent, Category = Combat)
	void AbilityInputReleased();

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	bool bIsTriggered = false;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	int32 InputIndex = 0;

	UFUNCTION(BlueprintCallable, Category = Combat)
	FVector GetMovementInput() const;

	UFUNCTION(BlueprintCallable, Category = Combat)
	FName GetLockTargetSectionName(const FVector& InVector) const;

	UFUNCTION(BlueprintCallable, Category = Combat)
	FVector GetLockTargetDir();

	UFUNCTION(BlueprintCallable, Category = Combat)
	void SetActorRotToTarget();
#pragma endregion

#pragma region 相机动画
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Ability|Camera", DisplayName="启用相机动画")
	bool bUseCameraAnimation = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Ability|Camera",
		meta=(EditCondition = bUseCameraAnimation, EditConditionHides), DisplayName="技能激活时执行相机动画")
	bool bExecuteCameraAnimWhenActivated = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Ability|Camera",
		meta=(EditCondition = bUseCameraAnimation, EditConditionHides), DisplayName="技能结束结束相机动画")
	bool bMatchWhenAbilityEnd = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Ability|Camera",
		meta=(EditCondition = bUseCameraAnimation, EditConditionHides), DisplayName="相机序列")
	UCameraAnimationSequence* CameraAnimationSequence;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Ability|Camera",
		meta=(EditCondition = bUseCameraAnimation, EditConditionHides), DisplayName="相机参数")
	FCameraAnimationParams CameraParams;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Ability|Camera",
		meta=(EditCondition = bUseCameraAnimation, EditConditionHides), DisplayName="停止时混出")
	bool bImmediate = false;
	void PlayCameraAnim(UCameraAnimationSequence* AnimationSequence, const FCameraAnimationParams& Params) const;
	UFUNCTION(BlueprintCallable, Category = "Cancer|Camera")
	void ExecuteCameraAnim();
#pragma endregion


	UFUNCTION(BlueprintCallable, Category = Combat)
	void AddTag(FGameplayTag Tag, int Count = 1);
	UFUNCTION(BlueprintCallable, Category = Combat)
	void RemoveTag(FGameplayTag Tag, int Count = 1);
	UFUNCTION(BlueprintCallable, Category = Combat)
	void SetTag(FGameplayTag Tag, int Count = 1);

protected:
	// Current camera mode set by the ability.
	TSubclassOf<UCancerCameraMode> ActiveCameraMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat Ability|Debug", DisplayName="EnableDebug")
	bool bEnableDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat Ability|Debug", DisplayName="DebugTime",
		meta=(EditCondition = "bEnableDebug", EditConditionHides))
	float EnableDebugTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat Ability", DisplayName="激活时等待事件")
	bool bActivateWaitEvent = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Combat Ability|Animation", DisplayName="动画完成结束技能")
	bool bMatchAnimationEnd{true};

private:
	FCancerAbilityDataSet AbilityData;

	mutable FGameplayTagContainer TempCooldownTags;

public:
	void SetAbilityData(FCancerAbilityDataSet InAbilityData);

	const FCancerAbilityDataSet& GetAbilityData() const;

	void AddTriggerData(TEnumAsByte<EGameplayAbilityTriggerSource::Type> InSource, FGameplayTag InTriggerTag);
};
