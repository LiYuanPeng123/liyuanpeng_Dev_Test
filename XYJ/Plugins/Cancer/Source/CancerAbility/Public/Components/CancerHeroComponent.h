#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "UIExtensionSystem.h"
#include "Components/ActorComponent.h"
#include "CancerHeroComponent.generated.h"

class UCancerAbilitySystemComponent;
class UCancerCameraMode;
class UCancerMeleeScan;

UCLASS(ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent))
class CANCERABILITY_API UCancerHeroComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCancerHeroComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintNativeEvent)
	void InitializeWithAbilitySystem();
	
#pragma region Boss相关
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<class UUserWidget> BossWidgetClass;
	UPROPERTY(BlueprintReadOnly)
	bool bIsShowBossWidget = true;
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag SlotID;
	UPROPERTY(BlueprintReadOnly)
	FUIExtensionHandle ExtensionHandle;

#pragma endregion

#pragma region 连击
	UPROPERTY()
	int32 ComboIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InterpSpeed = 10;

	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Combo")
	void IncrementCombo(int32 Step = 1);

	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Combo")
	void ResetCombo();
	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Combo")
	int32 GetComboIndex() const
	{
		return ComboIndex;
	}

#pragma endregion

#pragma region 战斗状态
	UFUNCTION(BlueprintNativeEvent)
	void OnHandleTrigger(const FGameplayTag Tag, int32 NewCount);
	UFUNCTION(BlueprintNativeEvent)
	void OnHandleExit(const FGameplayTag Tag, int32 NewCount);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="进入状态标签")
	FGameplayTagContainer TriggerTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="退出状态标签")
	FGameplayTagContainer ExitTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="战斗状态标签")
	FGameplayTag ComboTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="退出时间")
	float ExitCombatTime = 5.f;
	
	UPROPERTY()
	float ExitCombatRemainingTime = 0.f;
	
	UPROPERTY()
	bool bWeaponDrawn = false;
	
	UFUNCTION(BlueprintCallable)
	void ExecuteDrawWeapon();
	UFUNCTION(BlueprintCallable)
	void ExecutePutAwayWeapon();
	
	//进入战斗
	UFUNCTION(BlueprintNativeEvent)
	void OnTriggerEnterCombo();
	//离开战斗
	UFUNCTION(BlueprintNativeEvent)
	void OnTriggerExitCombo();
 	
#pragma endregion

#pragma region 连续格挡
	UPROPERTY()
	int32 BlockIndex = 0;

	int32 MaxBlockIndex{3};

	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Combo")
	bool IncrementBlock(int32 Step = 1);

	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Combo")
	void ResetBlock();

	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Combo")
	int32 GetBlockIndex() const { return BlockIndex; }

	FTimerHandle IncrementBlockTimerHandle;

#pragma endregion

#pragma region  摄像机

	UFUNCTION(BlueprintPure, Category = "Cancer|Hero")
	static UCancerHeroComponent* FindHeroComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UCancerHeroComponent>() : nullptr);
	}

	//-----------技能摄像机模式------------
	/** 设置技能摄像机模式 */
	void SetAbilityCameraMode(TSubclassOf<UCancerCameraMode> CameraMode,
	                          const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** 取消技能摄像机模式设置 */
	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

	UFUNCTION()
	TSubclassOf<UCancerCameraMode> DetermineCameraMode() const;
	//-----------技能摄像机模式------------

	virtual void OnUnregister() override;
#pragma endregion

#pragma region 目标

public:
	UPROPERTY()
	AActor* CombatTarget;
	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Components|Combat Manager")
	const AActor* GetCombatTarget();
	
	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Components|Combat Manager")
	void SetCombatTarget(AActor* NewCombatTarget);
	
	bool HasCombatTarget() const;

	void RotateToTarget( float FrameDeltaTime);
	
#pragma endregion

#pragma region 伤害注册

public:
	//注册伤害检测实例
	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Components|Combat Manager")
	int32 RegisterMeleeScanRequest(UCancerMeleeScan* MeleeScan);
	//取消伤害检测（伤害检测实例结束是会自行调用）
	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Components|Combat Manager")
	void CancelMeleeScanRequest(int32 RequestId);
#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "Cancer Combat|Components")
	UAbilitySystemComponent* GetAbilitySystemComponentFromOwner() const;

protected:
	UPROPERTY()
	TObjectPtr<UCancerAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TMap<int32, TObjectPtr<UCancerMeleeScan>> MeleeScanMap;

	TMap<int32, FTimerHandle> MeleeScanTimerHandles;

	UPROPERTY()
	TSubclassOf<UCancerCameraMode> AbilityCameraMode;

	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;
};
