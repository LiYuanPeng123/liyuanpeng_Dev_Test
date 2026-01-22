#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "CombatAbility_JumpBase.generated.h"

class UAbilityTask_WaitInputRelease;
/**
 * 跳跃系能力的公共基类：
 */
UCLASS(Abstract)
class CANCERABILITY_API UCombatAbility_JumpBase : public UCancerGameplayAbility
{
	GENERATED_BODY()

protected:
	// 是否覆盖垂直起跳速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Jump", meta = (DisplayName = "启用跳跃速度覆盖"))
	bool bOverrideJumpZVelocity = false;

	// 垂直起跳速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Jump",
		meta = (DisplayName = "跳跃速度(垂直)", ClampMin = "0.0"))
	float JumpZVelocityOverride = 600.0f;

	// 是否覆盖重力倍率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Jump", meta = (DisplayName = "启用重力速度覆盖"))
	bool bOverrideGravityScale = false;

	// 重力倍率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Jump",
		meta = (DisplayName = "重力速度(倍率)", ClampMin = "0.0"))
	float GravityScaleOverride = 1.0f;

	// 是否覆盖空中旋转速度(Yaw)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Jump",
		meta = (DisplayName = "启用空中旋转速度覆盖"))
	bool bOverrideAirRotationRateYaw = false;

	// 空中旋转速度(Yaw 度/秒)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Jump",
		meta = (DisplayName = "空中旋转速度(Yaw 度/秒)", ClampMin = "0.0"))
	float AirRotationRateYaw = 360.0f;

	// 是否覆盖跳跃加速度(MaxAcceleration)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Jump", meta = (DisplayName = "启用跳跃加速度覆盖"))
	bool bOverrideMaxAcceleration = false;

	// 跳跃加速度(MaxAcceleration)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Ability|Jump",
		meta = (DisplayName = "跳跃加速度(MaxAcceleration)", ClampMin = "0.0"))
	float MaxAccelerationOverride = 2024.0f;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
	                        const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

public:
	// 原值缓存（用于恢复）
	float PrevJumpZVelocity = 0.0f;
	float PrevGravityScale = 1.0f;
	float PrevMaxAcceleration = 0.0f;
	FRotator PrevRotationRate = FRotator::ZeroRotator;


	// 应用与恢复（供子类调用）
	UFUNCTION(BlueprintCallable)
	void ApplyJumpMovementOverrides(const AActor* ActorInfo);
	UFUNCTION(BlueprintCallable)
	void RestoreJumpMovementOverrides(const AActor* ActorInfo);
};
