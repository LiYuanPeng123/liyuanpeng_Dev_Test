#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PredictiveMontageTask.generated.h"

class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPredictiveMontageSimpleDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPredictiveMontageCorrection, float, ServerTime);

/**
 * 可预测蒙太奇任务（最小版骨架）
 * - 在能力内统一封装播放/跳段/结束回调
 * - 后续可扩展为客户端预测与服务端纠偏
 */
UCLASS()
class CANCERABILITY_API UPredictiveMontageTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPredictiveMontageSimpleDelegate OnStarted;

	UPROPERTY(BlueprintAssignable)
	FPredictiveMontageSimpleDelegate OnEnded;

	UPROPERTY(BlueprintAssignable)
	FPredictiveMontageSimpleDelegate OnCancelled;

	UPROPERTY(BlueprintAssignable)
	FPredictiveMontageCorrection OnServerCorrection;

	UFUNCTION(BlueprintCallable, meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UPredictiveMontageTask* PlayPredictiveMontage(UGameplayAbility* OwningAbility, UAnimMontage* Montage, FName Section, float PlayRate, float RootMotionScale);

	UFUNCTION(BlueprintCallable)
	void JumpToSection(FName Section);

	UFUNCTION(BlueprintCallable)
	void SetPlayRate(float InPlayRate);

	UFUNCTION(BlueprintCallable)
	void SetRootMotionScale(float InScale);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	UPROPERTY()
	UAnimMontage* MontageRef;

	FName SectionName = NAME_None;
	float PlayRate = 1.f;
	float RootMotionScale = 1.f;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
