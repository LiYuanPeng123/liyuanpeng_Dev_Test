#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ComboMontagePlayer.generated.h"

class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FComboMontageSimpleDelegate);

/**
 * 蒙太奇播放器（独立）
 * - 负责在角色上播放指定蒙太奇与 Section
 * - 提供跳段、播放速率与根运动缩放接口
 * - 广播开始/结束/取消事件
 */
UCLASS(ClassGroup=(Combo), meta=(BlueprintSpawnableComponent))
class CANCERABILITY_API UComboMontagePlayer : public UActorComponent
{
	GENERATED_BODY()
public:
	UComboMontagePlayer();

	UPROPERTY(BlueprintAssignable)
	FComboMontageSimpleDelegate OnStarted;
	UPROPERTY(BlueprintAssignable)
	FComboMontageSimpleDelegate OnEnded;
	UPROPERTY(BlueprintAssignable)
	FComboMontageSimpleDelegate OnCancelled;

	UFUNCTION(BlueprintCallable)
	bool Play(UAnimMontage* Montage, FName Section, float PlayRate, float RootMotionScale);

	UFUNCTION(BlueprintCallable)
	void JumpToSection(FName Section);

	UFUNCTION(BlueprintCallable)
	void SetPlayRate(float InPlayRate);

	UFUNCTION(BlueprintCallable)
	void SetRootMotionScale(float InScale);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UAnimInstance* AnimInstance;
	UPROPERTY()
	UAnimMontage* CurrentMontage;
	float CurrentPlayRate = 1.f;
	float CurrentRootMotionScale = 1.f;

	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};

