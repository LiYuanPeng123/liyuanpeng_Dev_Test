#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Combo/ComboInput.h"
#include "Combo/ComboPreInputArbitrationConfig.h"
#include "ComboController.generated.h"

/**
 * 连招控制器
 * - 维护当前招式段（MoveDefinition）
 * - 管理输入缓冲、状态标签
 * - 通过动画通知窗口（Combo/Cancel/Pre-Input）驱动派生与取消
 * - 在窗口开启时依据 Transition 匹配与优先级切换到下一段
 */
class UComboGraph;
class UMoveDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveStarted, UMoveDefinition*, Move);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveEnded, UMoveDefinition*, Move);

UCLASS(ClassGroup=(Combo), meta=(BlueprintSpawnableComponent))
class CANCERABILITY_API UComboController : public UActorComponent
{
	GENERATED_BODY()

public:
	UComboController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UComboGraph> ComboGraph;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo|PreInput")
	TObjectPtr<UComboPreInputArbitrationConfig> PreInputConfig;

	UPROPERTY(BlueprintAssignable)
	FOnMoveStarted OnMoveStarted;

	UPROPERTY(BlueprintAssignable)
	FOnMoveEnded OnMoveEnded;

	UFUNCTION(BlueprintCallable)
	void InitializeGraph(UComboGraph* InGraph);

	/** 启动连招（若未指定则使用图的起始段） */
	UFUNCTION(BlueprintCallable)
	void StartCombo(UMoveDefinition* StartMove);

	/** 写入已解析输入（窗口内直接缓冲；预输入窗口内暂存） */
	UFUNCTION(BlueprintCallable)
	void BufferInput(const FGameplayTagContainer& Input);

	/** 更新状态标签（例如空中/地面/锁定等） */
	UFUNCTION(BlueprintCallable)
	void SetStateTags(const FGameplayTagContainer& Tags);

	/** 打开连招窗口（允许派生） */
	UFUNCTION(BlueprintCallable)
	void NotifyComboWindowOpen();

	/** 关闭连招窗口（清空即时输入缓冲） */
	UFUNCTION(BlueprintCallable)
	void NotifyComboWindowClose();

	/** 打开取消窗口（允许闪避/格挡等取消） */
	UFUNCTION(BlueprintCallable)
	void NotifyCancelWindowOpen();

	/** 关闭取消窗口 */
	UFUNCTION(BlueprintCallable)
	void NotifyCancelWindowClose();

	/** 当前是否处于取消窗口 */
	UFUNCTION(BlueprintPure)
	bool IsCancelWindowOpen() const;

	/** 打开预输入窗口（允许提前输入并在窗口开启时自动消耗） */
	UFUNCTION(BlueprintCallable)
	void NotifyPreInputOpen();
	/** 关闭预输入窗口（清空预输入暂存） */
	UFUNCTION(BlueprintCallable)
	void NotifyPreInputClose();

	/** 在连招窗口内尝试派生下一段 */
	UFUNCTION(BlueprintCallable)
	bool TryAdvance();

	/** 获取当前招式段 */
	UFUNCTION(BlueprintPure)
	UMoveDefinition* GetCurrentMove() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UMoveDefinition> CurrentMove;

	UPROPERTY()
	FGameplayTagContainer StateTags;

	UPROPERTY()
	TArray<FGameplayTagContainer> InputBuffer;

	bool bComboWindowOpen = false;
	bool bCancelWindowOpen = false;
	bool bPreInputOpen = false;

	UPROPERTY()
	FGameplayTagContainer PreInputBuffer;

	UPROPERTY()
	TArray<FComboResolvedInput> AttackPreInputs;
	UPROPERTY()
	TArray<FComboResolvedInput> CancelPreInputs;

	void StagePreInput(const FComboResolvedInput& Input);
	void PurgeOldPreInputs();
	FGameplayTagContainer ConsumeBestPreInput(EComboWindowType WindowType);
	int32 ScorePreInput(const FComboResolvedInput& Input) const;
	bool IsCancelTag(const FGameplayTagContainer& Tags) const;
	bool IsAttackTag(const FGameplayTagContainer& Tags) const;

	UMoveDefinition* EvaluateNextMove() const;
	void ClearBuffer();
};
